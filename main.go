package main

import (
	"math"
	"bufio"
	"crypto/ed25519"
	"crypto/hmac"
	"crypto/sha512"
	"encoding/binary"
	"encoding/hex"
	"flag"
	"timeshare"
	"fmt"
	"os"
	"runtime"
	"runtime/pprof"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"

	"github.com/mr-tron/base58"
	"github.com/tyler-smith/go-bip39"
)

type DerivationPath struct {
	Name     string
	Template string
	Depth    int
}

type AddressData struct {
	Address    string
	PrivateKey string
	Path       string
}

type InputData struct {
	Value  string
	IsPriv bool
}


var (
	processedSeeds   int64
	totalSeeds       int64
	totalPrivKeys    int64
	generatedAddrs   int64
	lastUpdateAtomic int64
	startTime        time.Time
	mu               sync.Mutex
)

func main() {
	inputFile := flag.String("input", "seeds.txt", "Input file with seed phrases")
	outputAddr := flag.String("out-addr", "addresses.txt", "Output file with addresses")
	outputPriv := flag.String("out-priv", "addresses_privkeys.txt", "Output file with addresses;privkeys")
	solDepth := flag.Int("depth", 10, "Solana derivation depth (number of accounts)")
	workers := flag.Int("workers", runtime.NumCPU()*3, "Number of parallel workers")
	cpuprofile := flag.String("cpuprofile", "", "write cpu profile to file")
	flag.Parse()

	if *cpuprofile != "" {
		f, err := os.Create(*cpuprofile)
		if err != nil {
			fmt.Printf("Error creating CPU profile: %v\n", err)
			os.Exit(1)
		}
		defer f.Close()
		if err := pprof.StartCPUProfile(f); err != nil {
			fmt.Printf("Error starting CPU profile: %v\n", err)
			os.Exit(1)
		}
		defer pprof.StopCPUProfile()
	}

	runtime.GOMAXPROCS(*workers)

	inputData, err := readInputData(*inputFile)
	if err != nil {
		fmt.Printf("Error reading input: %v\n", err)
		os.Exit(1)
	}

	totalItems := int64(len(inputData))
	seedCount := totalItems - totalPrivKeys
	totalSeeds = totalItems
	if totalItems == 0 {
		fmt.Println("No valid input found")
		os.Exit(1)
	}

	fmt.Printf("Loaded %d seeds + %d private keys, Solana depth: %d, Workers: %d\n", seedCount, totalPrivKeys, *solDepth, *workers)

	addrFile, err := os.Create(*outputAddr)
	if err != nil {
		fmt.Printf("Error creating address file: %v\n", err)
		os.Exit(1)
	}
	defer addrFile.Close()

	privFile, err := os.Create(*outputPriv)
	if err != nil {
		fmt.Printf("Error creating privkey file: %v\n", err)
		os.Exit(1)
	}
	defer privFile.Close()

	addrWriter := bufio.NewWriterSize(addrFile, 1024*1024)
	privWriter := bufio.NewWriterSize(privFile, 1024*1024)
	defer addrWriter.Flush()
	defer privWriter.Flush()

	paths := getSolanaDerivationPaths(*solDepth)
	parsedPaths := parseFullPaths(paths)

	inputChan := make(chan InputData, *workers*2)
	resultChan := make(chan []AddressData, *workers*4)

	var wg sync.WaitGroup

	for i := 0; i < *workers; i++ {
		wg.Add(1)
		go worker(inputChan, resultChan, parsedPaths, &wg)
	}

	var writerWg sync.WaitGroup
	writerWg.Add(1)
	go writer(resultChan, addrWriter, privWriter, &writerWg)

	startTime = time.Now()
	atomic.StoreInt64(&lastUpdateAtomic, startTime.UnixMilli())
	go func() {
		for _, item := range inputData {
			inputChan <- item
		}
		close(inputChan)
	}()

	wg.Wait()
	close(resultChan)
	writerWg.Wait()

	elapsed := time.Since(startTime)
	fmt.Printf("\r\033[2K Done: %d items (%d seeds + %d privkeys) -> %d addresses in %v\n", totalSeeds, seedCount, totalPrivKeys, generatedAddrs, elapsed)
}

func worker(inputChan <-chan InputData, resultChan chan<- []AddressData, paths []FullParsedPath, wg *sync.WaitGroup) {
	defer wg.Done()

	for input := range inputChan {
		var addresses []AddressData
		if input.IsPriv {
			addr := privateKeyToAddress(input.Value)
			if addr != nil {
				addresses = append(addresses, *addr)
			}
		} else {
			addresses = generateSolanaAddresses(input.Value, paths)
		}
		if len(addresses) > 0 {
			resultChan <- addresses
		}
		atomic.AddInt64(&processedSeeds, 1)
		updateProgress()
	}
}

func writer(resultChan <-chan []AddressData, addrWriter, privWriter *bufio.Writer, wg *sync.WaitGroup) {
	defer wg.Done()

	for addresses := range resultChan {
		for _, addr := range addresses {
			addrWriter.WriteString(addr.Address)
			addrWriter.WriteByte('\n')

			privWriter.WriteString(addr.Address)
			privWriter.WriteByte(';')
			privWriter.WriteString(addr.PrivateKey)
			privWriter.WriteByte('\n')
		}
		atomic.AddInt64(&generatedAddrs, int64(len(addresses)))
	}
}

func updateProgress() {
	now := time.Now()
	nowMs := now.UnixMilli()

	lastTime := atomic.LoadInt64(&lastUpdateAtomic)
	if nowMs-lastTime < 500 {
		return
	}

	mu.Lock()
	lastTime = atomic.LoadInt64(&lastUpdateAtomic)
	if nowMs-lastTime < 500 {
		mu.Unlock()
		return
	}
	atomic.StoreInt64(&lastUpdateAtomic, nowMs)
	mu.Unlock()

	processed := atomic.LoadInt64(&processedSeeds)
	generated := atomic.LoadInt64(&generatedAddrs)
	percentage := float64(processed) / float64(totalSeeds) * 100
	elapsed := time.Since(startTime)

	fmt.Printf("\r\033[2KProcessing: %d/%d seeds (%.1f%%) -> %d addresses [%v]", processed, totalSeeds, percentage, generated, elapsed.Round(time.Second))
}

// SLIP-0010 Ed25519 master key derivation
func newMasterKey(seed []byte) ([]byte, []byte) {
	hmacHash := hmac.New(sha512.New, []byte("ed25519 seed"))
	hmacHash.Write(seed)
	sum := hmacHash.Sum(nil)
	return sum[:32], sum[32:]
}

// SLIP-0010 hardened child key derivation for Ed25519
func deriveChildKey(key, chainCode []byte, index uint32) ([]byte, []byte) {
	// Ed25519 only supports hardened derivation
	index = index | 0x80000000

	data := make([]byte, 37)
	data[0] = 0x00
	copy(data[1:33], key)
	binary.BigEndian.PutUint32(data[33:], index)

	hmacHash := hmac.New(sha512.New, chainCode)
	hmacHash.Write(data)
	sum := hmacHash.Sum(nil)

	return sum[:32], sum[32:]
}

func generateSolanaAddresses(mnemonic string, paths []FullParsedPath) []AddressData {
	trimmed := strings.TrimSpace(mnemonic)
	if !bip39.IsMnemonicValid(trimmed) {
		return nil
	}

	seed := bip39.NewSeed(trimmed, "")
	masterKey, masterChainCode := newMasterKey(seed)

	totalAddrs := 0
	for _, p := range paths {
		totalAddrs += p.Depth
	}
	addresses := make([]AddressData, 0, totalAddrs)

	for _, pathDef := range paths {
		for i := 0; i < pathDef.Depth; i++ {
			key, chainCode := masterKey, masterChainCode

			for _, seg := range pathDef.Segments {
				var index uint32
				if seg.IsN {
					index = uint32(i)
				} else {
					index = seg.Index
				}
				key, chainCode = deriveChildKey(key, chainCode, index)
			}

			privateKey := ed25519.NewKeyFromSeed(key)
			publicKey := privateKey.Public().(ed25519.PublicKey)
			address := base58.Encode(publicKey)
			privateKeyBase58 := base58.Encode(privateKey)

			addresses = append(addresses, AddressData{
				Address:    address,
				PrivateKey: privateKeyBase58,
				Path:       pathDef.Template,
			})
		}
	}

	return addresses
}

func readInputData(filename string) ([]InputData, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var data []InputData
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line != "" {
			isPriv := isSolanaPrivateKey(line)
			data = append(data, InputData{Value: line, IsPriv: isPriv})
			if isPriv {
				atomic.AddInt64(&totalPrivKeys, 1)
			}
		}
	}

	return data, scanner.Err()
}

func getSolanaDerivationPaths(depth int) []DerivationPath {
	return []DerivationPath{
		// Phantom, Solflare - стандартный путь
		{Name: "Solana Phantom/Solflare", Template: "m/44'/501'/n'/0'", Depth: depth},
		// Ledger Live стиль
		{Name: "Solana Ledger Live", Template: "m/44'/501'/0'/n'", Depth: depth},
		// Trust Wallet, мобильные кошельки
		{Name: "Solana Trust Wallet", Template: "m/44'/501'/n'", Depth: depth},
		// Sollet legacy
		{Name: "Solana Sollet Legacy", Template: "m/44'/501'/0'/0'/n'", Depth: depth},
		// Slope Wallet
		{Name: "Solana Slope", Template: "m/44'/501'/n'/0'/0'", Depth: depth},
		// Math Wallet
		{Name: "Solana Math Wallet", Template: "m/44'/501'/0'/n", Depth: depth},
		// Coin98
		{Name: "Solana Coin98", Template: "m/44'/501'/n'/0", Depth: depth},
		// Edge Wallet
		{Name: "Solana Edge", Template: "m/501'/n'/0'/0'", Depth: depth},
		// Альтернативные пути
		{Name: "Solana Alt 1", Template: "m/44'/501'/n", Depth: depth},
		{Name: "Solana Alt 2", Template: "m/501'/0'/0'/n'", Depth: depth},
		{Name: "Solana Alt 3", Template: "m/44'/501'/0'/0'/n", Depth: depth},
	}
}

type ParsedSegment struct {
	Index    uint32
	Hardened bool
	IsN      bool // true if this is the 'n' placeholder
}

type FullParsedPath struct {
	Name     string
	Template string
	Depth    int
	Segments []ParsedSegment
}

func parseFullPaths(paths []DerivationPath) []FullParsedPath {
	result := make([]FullParsedPath, 0, len(paths))

	for _, p := range paths {
		parts := strings.Split(p.Template, "/")
		if len(parts) < 2 {
			continue
		}

		segments := make([]ParsedSegment, 0, len(parts)-1)

		// Parse all segments after 'm'
		for _, element := range parts[1:] {
			var seg ParsedSegment

			if strings.Contains(element, "n") {
				seg.IsN = true
				seg.Hardened = strings.HasSuffix(element, "'")
			} else {
				seg.Hardened = strings.HasSuffix(element, "'")
				element = strings.TrimSuffix(element, "'")
				i, _ := strconv.Atoi(element)
				seg.Index = uint32(i)
			}
			segments = append(segments, seg)
		}

		depth := p.Depth
		if depth == 0 {
			depth = 1
		}

		result = append(result, FullParsedPath{
			Name:     p.Name,
			Template: p.Template,
			Depth:    depth,
			Segments: segments,
		})
	}
	return result
}

func isSolanaPrivateKey(s string) bool {
	s = strings.TrimSpace(s)

	// Try base58 decode (64 bytes = ed25519 full key, 32 bytes = seed only)
	decoded, err := base58.Decode(s)
	if err == nil && (len(decoded) == 64 || len(decoded) == 32) {
		return true
	}

	// Try hex decode
	if len(s) == 128 || len(s) == 64 {
		_, err := hex.DecodeString(s)
		if err == nil {
			return true
		}
	}

	// Try JSON array format [1,2,3,...] (common export format)
	if strings.HasPrefix(s, "[") && strings.HasSuffix(s, "]") {
		return true
	}

	return false
}

func privateKeyToAddress(privKeyStr string) *AddressData {
	privKeyStr = strings.TrimSpace(privKeyStr)

	var privateKey ed25519.PrivateKey

	// Try base58 decode
	if decoded, err := base58.Decode(privKeyStr); err == nil {
		if len(decoded) == 64 {
			privateKey = ed25519.PrivateKey(decoded)
		} else if len(decoded) == 32 {
			privateKey = ed25519.NewKeyFromSeed(decoded)
		}
	}

	// Try hex decode
	if privateKey == nil {
		if decoded, err := hex.DecodeString(privKeyStr); err == nil {
			if len(decoded) == 64 {
				privateKey = ed25519.PrivateKey(decoded)
			} else if len(decoded) == 32 {
				privateKey = ed25519.NewKeyFromSeed(decoded)
			}
		}
	}

	// Try JSON array format
	if privateKey == nil && strings.HasPrefix(privKeyStr, "[") {
		privKeyStr = strings.Trim(privKeyStr, "[]")
		parts := strings.Split(privKeyStr, ",")
		if len(parts) == 64 || len(parts) == 32 {
			bytes := make([]byte, len(parts))
			valid := true
			for i, p := range parts {
				p = strings.TrimSpace(p)
				val, err := strconv.Atoi(p)
				if err != nil || val < 0 || val > 255 {
					valid = false
					break
				}
				bytes[i] = byte(val)
			}
			if valid {
				if len(bytes) == 64 {
					privateKey = ed25519.PrivateKey(bytes)
				} else {
					privateKey = ed25519.NewKeyFromSeed(bytes)
				}
			}
		}
	}

	if privateKey == nil {
		return nil
	}

	publicKey := privateKey.Public().(ed25519.PublicKey)
	address := base58.Encode(publicKey)
	privateKeyBase58 := base58.Encode(privateKey)

	return &AddressData{
		Address:    address,
		PrivateKey: privateKeyBase58,
		Path:       "direct",
	}
}
