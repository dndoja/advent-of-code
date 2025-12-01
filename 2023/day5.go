package main

import (
	"bufio"
	"fmt"
	"strconv"
	"strings"
)

type Mapping struct {
	start     int
	end       int
	transform int
}

func RunDay5(scanner *bufio.Scanner) {
	dataRaw := ""
	for scanner.Scan() {
		line := scanner.Text()
		dataRaw += line + "\n"
	}

	seedsRaw, mappings := parseSeedsAndMappings(dataRaw)
	seeds := []Mapping{}
	seedsExpanded := expandSeeds(seedsRaw)
	for _, seed := range seedsRaw {
		seeds = append(seeds, Mapping{
			start: seed,
			end:   seed,
		})
	}

	minLocationPt1 := getMinLocation(seeds, mappings)
	minLocationPt2 := getMinLocation(seedsExpanded, mappings)

	fmt.Println(minLocationPt1)
	fmt.Println(minLocationPt2)
}

func expandSeeds(seeds []int) []Mapping {
	expanded := []Mapping{}

	for i := 0; i < len(seeds)-1; i += 2 {
		start := seeds[i]
		count := seeds[i+1]

		expanded = append(expanded, Mapping{
			start: start,
			end:   start + count - 1,
		})
	}

	return expanded
}

func parseSeedsAndMappings(dataRaw string) (seeds []int, mappings [][]Mapping) {
	blocks := strings.Split(dataRaw, "\n\n")

	seedsRaw := strings.Fields(strings.TrimPrefix(blocks[0], "seeds:"))
	seeds = []int{}

	for i := 0; i < len(seedsRaw); i++ {
		seed, err := strconv.Atoi(seedsRaw[i])
		if err == nil {
			seeds = append(seeds, seed)
		}
	}

	mappings = [][]Mapping{}

	for i := 1; i < len(blocks); i++ {
		block := blocks[i]
		lines := strings.Split(block, "\n")
		blockMappings := []Mapping{}

		for lineIndex := 1; lineIndex < len(lines); lineIndex++ {
			lineElements := strings.Fields(lines[lineIndex])
			if len(lineElements) < 3 {
				continue
			}

			dest, destErr := strconv.Atoi(lineElements[0])
			src, srcErr := strconv.Atoi(lineElements[1])
			count, countErr := strconv.Atoi(lineElements[2])

			if srcErr != nil || destErr != nil || countErr != nil {
				continue
			}

			blockMappings = append(blockMappings, Mapping{src, src + count - 1, dest - src})
		}
		mappings = append(mappings, blockMappings)
	}

	return seeds, mappings
}

func getMinLocation(seedRanges []Mapping, mappings [][]Mapping) int {
	minLocation := int(^uint(0) >> 1)

	mappedRanges := seedRanges

	for layer := 0; layer < len(mappings); layer++ {
		mappingsInLayer := []Mapping{}

		for _, currentRange := range mappedRanges {
			mappedSegments := []Mapping{}
			unmappedSegments := []Mapping{currentRange}

			for _, mapping := range mappings[layer] {
				for i, segment := range unmappedSegments {
					intersectsLeft := belongsInRange(segment.start, mapping)
					intersectsRight := belongsInRange(segment.end, mapping)

					if intersectsLeft && intersectsRight {
						unmappedSegments = removeFromSlice(i, unmappedSegments)
						mappedSegments = append(mappedSegments, transformMapping(
							segment.start,
							segment.end,
							mapping.transform,
						))
					} else if intersectsLeft {
						unmappedSegments = removeFromSlice(i, unmappedSegments)
						unmappedSegments = append(unmappedSegments, Mapping{
							start: mapping.end + 1,
							end:   segment.end,
						})
						mappedSegments = append(mappedSegments, transformMapping(
							segment.start,
							mapping.end,
							segment.transform+mapping.transform,
						))
					} else if intersectsRight {
						unmappedSegments = removeFromSlice(i, unmappedSegments)
						unmappedSegments = append(unmappedSegments, Mapping{
							start: segment.start,
							end:   mapping.start - 1,
						})
						mappedSegments = append(mappedSegments, transformMapping(
							mapping.start,
							segment.end,
							mapping.transform,
						))
					} else if segment.start < mapping.start && segment.end > mapping.end {
						// Case where segment is bigger than mapping
						unmappedSegments = removeFromSlice(i, unmappedSegments)
						unmappedSegments = append(unmappedSegments, Mapping{
							start: segment.start,
							end:   mapping.start - 1,
						})
						unmappedSegments = append(unmappedSegments, Mapping{
							start: mapping.end + 1,
							end:   segment.end,
						})
						mappedSegments = append(mappedSegments, transformMapping(
							mapping.start,
							mapping.end,
							mapping.transform,
						))
					}
				}
			}

			mappingsInLayer = append(mappingsInLayer, mappedSegments...)
			mappingsInLayer = append(mappingsInLayer, unmappedSegments...)
		}

		mappedRanges = mappingsInLayer
	}

	for _, mappedRange := range mappedRanges {
		if mappedRange.start < minLocation {
			minLocation = mappedRange.start
		}
	}

	return minLocation
}

func transformMapping(start int, end int, transform int) Mapping {
	return Mapping{
		start: start + transform,
		end:   end + transform,
	}
}

func removeFromSlice[T any](index int, slice []T) []T {
	return append(slice[:index], slice[index+1:]...)
}

func belongsInRange(number int, mRange Mapping) bool {
	return number >= mRange.start && number <= mRange.end
}
