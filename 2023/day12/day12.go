package day12

import (
	"bufio"
	"fmt"
	"math"
	"slices"
	"strconv"
	"strings"
)

type Islands = []int

type ConditionRecord struct {
	chars  []byte
	groups []int
}

type Node struct {
	chars    []byte
	offset   int
	level    int
	children []*Node
	islands  Islands
	previous *Node
}

type Layer struct {
	start            int
    end              int
	mask             []int
	occupiedHashtags []bool
}

func (record *ConditionRecord) getEmptyLayers() []Layer {
	layers := make([]Layer, len(record.groups))
	for i := range layers {
		layers[i] = NewLayer(len(record.chars))
	}

	return layers
}

func NewLayer(size int) Layer {
	return Layer{
        start: 0,
        end: size-1, 
        mask: make([]int, size), 
        occupiedHashtags: make([]bool, size),
    }
}

const verbose = false
const expand = false

func (node *Node) print() {
	fmt.Print(" ")
	if node.level == 0 {
		fmt.Println("")
	}
	if node.previous != nil {
		node.previous.print()
		fmt.Print(" > ")
	}
	for _, char := range node.chars {
		fmt.Print(string(char))
	}
	if node.level > -1 {
		fmt.Printf("[%d:%d] ", node.level, node.offset)
	}
	// fmt.Print(node.islands)
}

func printLayers(layers []Layer) {
	fmt.Println("")
	for _, l := range layers {
		for _, v := range l.mask {
			if v >= 0 && v < 10 {
				fmt.Print(" ", v, "  ")
			} else {
				suffix := " "

				if v >= 0 {
					suffix += " "
				}

				if v > -10 && v < 10 {
					suffix += " "
				}
				fmt.Print(v, suffix)
			}
		}
        fmt.Print("    ", l.start, ":", l.end)
		fmt.Println("")
	}
	fmt.Println("")
}

func (node *Node) fullResult(record ConditionRecord) string {
	chars := append([]byte{}, record.chars...)

	chain := []*Node{}
	current := node

	for current != nil {
		chain = append(chain, current)
		for i := 0; i < len(current.chars); i++ {
			chars[current.offset+i] = current.chars[i]
		}

		current = current.previous
	}

	for i, char := range chars {
		if char == '?' {
			chars[i] = '.'
		}
	}

	return string(chars)
}

func stringRecord(record ConditionRecord) ConditionRecord {
	expanded := []ConditionRecord{}

	const multiplier int = 5
	fullchars := record.chars
	fulgroups := []int{}

	for i := 0; i < multiplier; i++ {
		chars := record.chars
		if i < multiplier-1 {
			fullchars = append(chars, '?')
		}
        fullchars = append(fullchars, chars...)

		expanded = append(expanded, ConditionRecord{chars, record.groups})
		fulgroups = append(fulgroups, record.groups...)
	}

	return ConditionRecord{fullchars, fulgroups}
}

func expandRecord(record ConditionRecord) []ConditionRecord {
	expanded := []ConditionRecord{}

	const multiplier int = 5

	for i := 0; i < multiplier; i++ {
		chars := record.chars

		if i < multiplier-1 {
			chars = append(chars, '?')
		}

		expanded = append(expanded, ConditionRecord{chars, record.groups})
	}

	return expanded
}

func Run(scanner *bufio.Scanner) {
	// ??#??#???#????????? 10,2,1,1

	// ?###???????? 3,2,1

	// for each group
	// 1. Items in window can satisfy group req
	// 2. Items after window can (potentially) satisfy other group constraints
	// Slide (n+1) window and check if two constraints are satisfied:

	records := []ConditionRecord{}

	var totalCombinations uint64 = 0
	var totalCombinationsOld uint64 = 0

	for scanner.Scan() {
		line := scanner.Text()
		records = append(records, parseConditionRecord(line))
	}

	//records = []ConditionRecord{records[5]}
	expandedRecords := []ConditionRecord{}

	for _, record := range records {
		if expand {
			expandedRecords = append(expandedRecords, stringRecord(record))
		} else {
			expandedRecords = append(expandedRecords, record)
		}
	}

	for _, record := range expandedRecords {
		fmt.Println(string(record.chars), record.groups)
		fmt.Println(record.groups)
		fmt.Println("")
		for i := range record.chars {
			if i < 10 {
				fmt.Print(" ", i, "  ")
			} else {
				fmt.Print(i, "  ")
			}
		}
		fmt.Println("")

		for _, ch := range record.chars {
			fmt.Print(" ", string(ch), "  ")
		}
		fmt.Println("")
		// possibleCombos2 := getPossibleCombinations(record)
		// totalCombinationsOld += uint64(len(possibleCombos2))

        positionLayers := getValidPositionLayers(record, nil)
        traversalLayers := getTraversalLayers(record, positionLayers)
        assignmentsVariations := getPoundPointAssignments(record, traversalLayers)

        groupCombinations := 0

        for _, assignments := range assignmentsVariations {
            positionLayers := getValidPositionLayers(record, &assignments)
            traversalLayers := getTraversalLayers(record, positionLayers)
            // fmt.Println(assignments)
            // printLayers(traversalLayers)
            _, combinations := getCombinationNumberLayers(
                record,
                positionLayers, 
                traversalLayers,
            )
            // printLayers(combinationNumberLayers)
            groupCombinations += combinations
        }

        totalCombinations += uint64(groupCombinations)

        // fmt.Println(groupCombinations, "/", len(possibleCombos2))
        // if groupCombinations != len(possibleCombos2) {
        //    fmt.Println("wrong", string(record.chars), record.groups)
        //}
		// fmt.Println("")
	}

	fmt.Println(totalCombinations)
	fmt.Println(totalCombinationsOld)
}

func getValidPositionLayers(record ConditionRecord, poundPointAssignments *map[int]Bounds) []Layer {
	layers := record.getEmptyLayers()
	scanStartOffset := 0

	for groupIndex, groupSize := range record.groups {
		lastIndex := len(record.chars) - groupSize
		firstValidPosition := -1

        if poundPointAssignments != nil {
            if poundPoint, ok := (*poundPointAssignments)[groupIndex]; ok {
                scanStartOffset = poundPoint.start
                if scanStartOffset < 0 { 
                    scanStartOffset = 0
                }
                if poundPoint.end < lastIndex { 
                    lastIndex = poundPoint.end
                }
            }
        }

		for i := scanStartOffset; i <= lastIndex; i++ {
			if i+groupSize > len(record.chars) {
				break
			}

			window := record.chars[i : i+groupSize]

			containsDots := false
			for _, c := range window {
                if c == '.' {
                    containsDots = true
                    break
                }
			}

			isValidPosition := (i == 0 || record.chars[i-1] != '#') &&
				(i == lastIndex || record.chars[i+groupSize] != '#') &&
				!containsDots

			if isValidPosition {
				color := 1
				layers[groupIndex].mask[i] = color

				if firstValidPosition == -1 {
					firstValidPosition = i
				}
			}
		}

		scanStartOffset = firstValidPosition + groupSize + 1
        layers[groupIndex].start = firstValidPosition
        layers[groupIndex].end = lastIndex
	}

    for layerIndex:=len(layers)-1;layerIndex>=1;layerIndex-- {
        layer := &layers[layerIndex]
        nextLayer := &layers[layerIndex-1]
        nextLayerGroupSize := record.groups[layerIndex-1]
        nextLayerMaxX := layer.end - nextLayerGroupSize - 1

        for x:=len(nextLayer.mask)-1; x>=0; x-- {
            if nextLayer.mask[x] == 0 { continue }

            if x > nextLayerMaxX {
                nextLayer.mask[x] = 0
            }else{
                nextLayer.end = x
                break
            }
        }
    }

	return layers
}

func getTraversalLayers(record ConditionRecord, sourceLayers []Layer) []Layer {
    layers := record.getEmptyLayers()

    for layerIndex, layer := range sourceLayers {
		position := 0
		positionX := -1

		for x, val := range layer.mask {
			if val > 0 {
				position++
				positionX = x
				layers[layerIndex].mask[x] = position
			} else if positionX != -1 {
				layers[layerIndex].mask[x] = positionX - x
			}
		}

        layers[layerIndex].start = layer.start
        layers[layerIndex].end = layer.end
    }

    return layers
}

func getCombinationNumberLayers(
    record ConditionRecord,
    positionLayers []Layer,
    traversalLayers []Layer,
) ([]Layer,int) {
    permutationLayers := record.getEmptyLayers()
	permutationLayers[0] = traversalLayers[0]
    possibleCombinations := 0

	for layerIndex := 1; layerIndex < len(positionLayers); layerIndex++ {
		layer := traversalLayers[layerIndex]
		upperLayerSize := record.groups[layerIndex-1]
		prevPositionPermutations := 0

		for i, validPositionsCount := range layer.mask {
			if validPositionsCount <= 0 {
				continue
			}

			x := i - upperLayerSize - 1
			if x < 0 {
				continue
			}

			if traversalLayers[layerIndex-1].mask[x] < 0 {
				x += traversalLayers[layerIndex-1].mask[x]
			}

            upperPermutations := permutationLayers[layerIndex-1].mask[x]
			
			permutations := upperPermutations
            permutations += prevPositionPermutations
            prevPositionPermutations = permutations
			permutationLayers[layerIndex].mask[i] = permutations
			possibleCombinations = permutations
		}

        if layerIndex == len(positionLayers)-1 && prevPositionPermutations == 0 {
            possibleCombinations = 0
        }
	}

    return permutationLayers, possibleCombinations
}

type PoundPointTaker struct {
    poundPoint int
    layerIndex int
    children []*PoundPointTaker
    availablePoundPoints map[int]bool
    implicitlyTakenPoundPoints map[int]bool
    assignments map[int]int
}

type Bounds struct {
    start int
    end   int
}

func getPoundPointAssignments(record ConditionRecord, traversalLayers []Layer) []map[int]Bounds {
    layersAndPoundPoints := make(map[int][]int, len(traversalLayers))
    poundPointsAndLayers := map[int][]bool{}

    poundPoints := []int {}
    poundPointsMap := map[int]bool {}
    for x, ch := range record.chars {
        if ch == '#' {
            poundPoints = append(poundPoints, x)
            poundPointsMap[x] = true
        }
    }
    
    prevPp4Layer := map[int]int{}

    for _, poundPoint := range poundPoints {
        for layerIndex, layer := range traversalLayers {
            layerWindowSize := record.groups[layerIndex]

            if poundPoint < layer.start || poundPoint > layer.end + layerWindowSize - 1 {
                continue
            }

            x := poundPoint
            if layer.mask[x] < 0 {
                x += layer.mask[x]
            }

            if x + layerWindowSize - 1 >= poundPoint {
                if _, found := poundPointsAndLayers[poundPoint]; !found {
                    poundPointsAndLayers[poundPoint] = make([]bool, len(traversalLayers))
                }

                prevPP, found := prevPp4Layer[layerIndex]

                if found && prevPP+1<poundPoint{
                    poundPointsAndLayers[poundPoint][layerIndex] = true

                    layersAndPoundPoints[layerIndex] = append(
                        layersAndPoundPoints[layerIndex],
                        poundPoint,
                    )
                }

                prevPp4Layer[layerIndex] = poundPoint
            }
        }
    }

    // fmt.Println("Layers+PPs", layersAndPoundPoints)
    
    root := PoundPointTaker{
        layerIndex: -1,
        availablePoundPoints: poundPointsMap,
        poundPoint: -1,
    }
    children := []*PoundPointTaker{&root}
    leaves := []*PoundPointTaker{}
    iter := 0
    for len(children) > 0 {
        current := children[0]
        children = children[1:]
        fmt.Println(iter)
        iter++

        if current.layerIndex == len(traversalLayers) - 1 {
            leaves = append(leaves, current)
        }

        isLeaf := true
        for nextLayer:=current.layerIndex+1; nextLayer<len(traversalLayers); nextLayer++{
            foundChildren := false
            for _, poundPoint := range layersAndPoundPoints[nextLayer] {
                if !current.availablePoundPoints[poundPoint] { continue }

                availablePoundPoints := map[int]bool {}
                implicitlyTakenPoundPoints := map[int]bool {}
                for p, available := range current.availablePoundPoints {
                    if poundPoint != p {
                        availablePoundPoints[p] = available
                        if p < poundPoint + record.groups[nextLayer] {
                            implicitlyTakenPoundPoints[p] = true
                        }
                    }else{
                        implicitlyTakenPoundPoints[p] = false
                    }
                }

                newAssignments := map[int]int {}
                for k,v := range current.assignments {
                    newAssignments[k] = v
                }
                newAssignments[nextLayer] = poundPoint

                canBeTakenInTheFuture := false
                for layer,isAvailable := range poundPointsAndLayers[poundPoint] {
                    if isAvailable && layer > nextLayer {
                        canBeTakenInTheFuture = true 
                        break
                    }
                }

                if canBeTakenInTheFuture || implicitlyTakenPoundPoints[poundPoint] || true {
                    emptyNode := PoundPointTaker {
                        availablePoundPoints: current.availablePoundPoints,
                        implicitlyTakenPoundPoints: implicitlyTakenPoundPoints,
                        assignments: current.assignments,
                        layerIndex: nextLayer,
                        poundPoint: -1,
                    }
                    children = append(children, &emptyNode)
                    current.children = append(current.children, &emptyNode)
                }

                newNode := PoundPointTaker{
                    availablePoundPoints: availablePoundPoints,
                    assignments: newAssignments,
                    implicitlyTakenPoundPoints: implicitlyTakenPoundPoints,
                    layerIndex: nextLayer,
                    poundPoint: poundPoint,
                }

                foundChildren = true
                children = append(children, &newNode)
                current.children = append(current.children, &newNode)
            }

            if foundChildren  { 
                isLeaf = false
                break 
            }
        }

        if isLeaf { leaves = append(leaves, current) }
    }
    
    assignments := []map[int]int {}
    assignmentFootprints := map[string]bool {}
    boundsVariations := []map[int]Bounds{}


    for _, leaf := range leaves {
        prevAssignedPoundPoint := -1
        bounds := map[int]Bounds {}
        assignedPoundPoints := map[int]bool {}    

        isValid := true
        for layerIndex := range traversalLayers {
            poundPoint, found := leaf.assignments[layerIndex]
            if !found { continue }
            layerSize := record.groups[layerIndex]
            layerBounds := Bounds{poundPoint-layerSize+1, poundPoint}

            if poundPoint < prevAssignedPoundPoint {
                isValid = false
                break
            }

            nextPoundPoint := math.MaxInt
            for nextLayer:=layerIndex+1; nextLayer<len(traversalLayers); nextLayer++ {
                if npp, found := leaf.assignments[nextLayer]; found {
                    nextPoundPoint = npp
                    break
                }
            }

            lastPoundPointInGroup := poundPoint
            for _, unassignedPoundPoint := range poundPoints {
                if assignedPoundPoints[unassignedPoundPoint] { continue }

                if unassignedPoundPoint < nextPoundPoint && 
                    (unassignedPoundPoint >= poundPoint &&
                        unassignedPoundPoint < poundPoint + layerSize) {

                    // fmt.Println(layerIndex, poundPoint, unassignedPoundPoint, nextPoundPoint)
                    lastPoundPointInGroup = unassignedPoundPoint
                    assignedPoundPoints[unassignedPoundPoint] = true
                }
            }

            layerBounds.start = lastPoundPointInGroup - layerSize + 1

            // for _, unassignedPoundPoint := range poundPoints {
            //     if assignedPoundPoints[unassignedPoundPoint] { continue }

            //     if unassignedPoundPoint < poundPoint && 
            //         unassignedPoundPoint >= layerBounds.start {
            //         assignedPoundPoints[unassignedPoundPoint] = true
            //     }
            // }
            bounds[layerIndex] = layerBounds
        }

        unassignedPoundPoints := []int {}
        for _, poundPoint := range poundPoints {
            if !assignedPoundPoints[poundPoint] {
                unassignedPoundPoints = append(unassignedPoundPoints, poundPoint)
            }
        }
        hasAssignedAllPoundPoints := len(assignedPoundPoints) == len(poundPoints)
        // fmt.Println("Assignments:", leaf.assignments, isValid, hasAssignedAllPoundPoints, unassignedPoundPoints)
        if isValid {
            if hasAssignedAllPoundPoints {
                // fmt.Println("Bounds", bounds)
                footprint := fmt.Sprint(bounds)
                if _, contains := assignmentFootprints[footprint]; !contains {
                    boundsVariations = append(boundsVariations, bounds)
                    assignments = append(assignments, leaf.assignments)
                    assignmentFootprints[footprint] = true
                }
            }
        }
    }


    fmt.Println("")

    return boundsVariations
}

func getLayers(record ConditionRecord, log bool) []Layer {
	possibleCombinations := 0
	colored := make([]int, len(record.chars))

	layers := record.getEmptyLayers()

	scanStartOffset := 0

	// Make a layer that contains all of the valid positions for each group
	for groupIndex, groupSize := range record.groups {
		lastIndex := len(record.chars) - groupSize
		scanEnd := lastIndex
		firstValidPosition := -1
		firstHashtagIndex := -1

		for i := scanStartOffset; i <= scanEnd; i++ {
			if i+groupSize > len(record.chars) {
				break
			}

			window := record.chars[i : i+groupSize]

			containsDots := false
			for x, c := range window {
				switch c {
				case '.':
					containsDots = true
				case '#':
					if firstHashtagIndex == -1 {
						firstHashtagIndex = i + x
						// scanEnd = firstHashtagIndex+1
					}
				}
			}

			isValidPosition := (i == 0 || record.chars[i-1] != '#') &&
				(i == lastIndex || record.chars[i+groupSize] != '#') &&
				!containsDots

			if isValidPosition {
				color := 1
				if firstHashtagIndex != -1 && i > firstHashtagIndex-groupSize &&
					i < firstHashtagIndex+groupSize {
					layers[groupIndex].occupiedHashtags[firstHashtagIndex] = true
					color = -(firstHashtagIndex + 1)
				}
				layers[groupIndex].mask[i] = color

				if firstValidPosition == -1 {
					firstValidPosition = i
				}
			}

			if firstHashtagIndex != -1 &&
				!isValidPosition {
				scanEnd = lastIndex
				firstHashtagIndex = -1
			}
		}

		scanStartOffset = firstValidPosition + groupSize + 1
		scanEnd = lastIndex
	}

	// Ensure that all existing # (which aren't ?) are always handled
	for layerIndex := len(layers) - 1; layerIndex >= 0; layerIndex-- {
		layer := layers[layerIndex]
		minX := -1
		for i, val := range layer.mask {
			if val >= 0 {
				continue
			}

			count := 0
			for _, layer := range layers {
				if layer.occupiedHashtags[-val-1] {
					count++
				}
			}

			if count != 1 {
				continue
			}

			for x := i; x < i+record.groups[layerIndex]; x++ {
				if record.chars[x] == '#' {
					minX = x
					break
				}
			}

			if minX != -1 {
				break
			}
		}

		if minX == -1 {
			continue
		}

		for i, val := range layer.mask {
			if val > 0 || i > minX {
				//layer.mask[i] = 0
			}
		}
	}

	if log {
		fmt.Println(record.groups)
		fmt.Println("")
		for i := range record.chars {
			if i < 10 {
				fmt.Print(" ", i, "  ")
			} else {
				fmt.Print(i, "  ")
			}
		}
		fmt.Println("")

		for _, ch := range record.chars {
			fmt.Print(" ", string(ch), "  ")
		}
		fmt.Println("")
	}

	// prevStart := -1

	validLocationLayers := record.getEmptyLayers()
	for i, layer := range layers {
		position := 0
		positionX := -1

		for x, val := range layer.mask {
			if val != 0 {
				position++
				positionX = x
				validLocationLayers[i].mask[x] = position
			} else if positionX != -1 {
				validLocationLayers[i].mask[x] = positionX - x
			}
		}
	}

	combinationNumberLayers := record.getEmptyLayers()
	lastValidLayerPositions := make([]int, len(layers))
	lastValidLayerPositions[len(layers)-1] = len(layers[0].mask)

	validPositionsByLayer := make([]map[int]int, len(layers))
	for i := range validPositionsByLayer {
		validPositionsByLayer[i] = map[int]int{}
	}

	for i := len(validLocationLayers) - 1; i >= 0; i-- {
		layer := validLocationLayers[i]

		var upperLayer *Layer
		var upperLayerWindowSize int
		if i > 0 {
			upperLayer = &validLocationLayers[i-1]
			upperLayerWindowSize = record.groups[i-1]
		}

		for x := layer.start; x < len(layer.mask); x++ {
			val := layer.mask[x]
			if val <= 0 {
				continue
			}

			_, hasValidPosition := validPositionsByLayer[i][val]
			if i < len(validLocationLayers)-1 && !hasValidPosition {
				continue
			}

			if i == 0 {
				combinationNumberLayers[i].mask[x] = val
				continue
			}

			for upperX := x - upperLayerWindowSize - 1; upperX >= upperLayer.start; upperX-- {
				upperVal := upperLayer.mask[upperX]
				if upperVal < 0 {
					upperX += upperVal + 1
					continue
				}

				if lastValidLayerPositions[i-1] == 0 || true {
					lastValidLayerPositions[i-1] = upperX
				}

				validPositionsByLayer[i-1][upperVal] = upperX
				combinationNumberLayers[i].mask[x] = upperVal
				break
			}
		}
	}

	hotpointLayers := record.getEmptyLayers()
	hotpoints := map[int][]bool{}
	hotpointKeys := []int{}
	mandatoryHotpoints := make([]int, len(layers))

	for i, layer := range layers {
		hotpointCounts := map[int]int{}
		lastHotpointX := -1

		for x, val := range layer.mask {
			if val < 0 && validLocationLayers[i].mask[x] > 0 && x <= lastValidLayerPositions[i] {
				hotpointCounts[val]++
				hotpointLayers[i].mask[x] = hotpointCounts[val]
				lastHotpointX = x
				if hotpointCounts[val] == 1 {
					hotpointX := val
					if _, ok := hotpoints[hotpointX]; !ok {
						hotpoints[hotpointX] = make([]bool, len(layers))
						hotpointKeys = append(hotpointKeys, hotpointX)
					}

					hotpoints[hotpointX][i] = true
				}
			} else if lastHotpointX != -1 {
				if val < 0 && x-lastHotpointX <= record.groups[i] {
					hotpointX := val
					if _, ok := hotpoints[hotpointX]; !ok {
						hotpoints[hotpointX] = make([]bool, len(layers))
						hotpointKeys = append(hotpointKeys, hotpointX)
					}

					hotpoints[hotpointX][i] = true
				}

				hotpointLayers[i].mask[x] = lastHotpointX - x
			}
		}
	}

	slices.Sort(hotpointKeys)
	slices.Reverse(hotpointKeys)

	for _, hotpoint := range hotpointKeys {
		layers := hotpoints[hotpoint]
		layersWithHotpoint := 0
		firstLayerWithHotpoint := -1
		for layer, hasHotpoint := range layers {
			if hasHotpoint {
				if firstLayerWithHotpoint == -1 {
					firstLayerWithHotpoint = layer
				}
				layersWithHotpoint++
			}
		}

		if layersWithHotpoint == 1 {
			for otherHotpoint, layers := range hotpoints {
				hotpointX := -hotpoint - 1
				otherHotpointX := -otherHotpoint - 1
				layerWindowSize := record.groups[firstLayerWithHotpoint]

				if otherHotpointX < hotpointX+layerWindowSize && otherHotpointX >= hotpointX {
					continue
				}

				layers[firstLayerWithHotpoint] = false
			}
		}
	}

	for _, hotpoint := range hotpointKeys {
		layers := hotpoints[hotpoint]
		layersWithHotpoint := 0
		firstLayerWithHotpoint := -1
		for layer, hasHotpoint := range layers {
			if hasHotpoint {
				if firstLayerWithHotpoint == -1 {
					firstLayerWithHotpoint = layer
				}
				layersWithHotpoint++
			}
		}

		if layersWithHotpoint == 1 {
			mandatoryHotpoints[firstLayerWithHotpoint] = hotpoint
		}
	}

	traversalLayers := record.getEmptyLayers()
	for i, layer := range validLocationLayers {
		position := 0
		positionX := -1
		lastX := lastValidLayerPositions[i]
		mandatoryHotpoint := mandatoryHotpoints[i]

		for x, val := range layer.mask {
			if val > 0 && x <= lastX &&
				(mandatoryHotpoint == 0 || layers[i].mask[x] == mandatoryHotpoint || true) {
				position++
				positionX = x
				traversalLayers[i].mask[x] = position
			} else if positionX != -1 {
				traversalLayers[i].mask[x] = positionX - x
			}
		}
	}

	return traversalLayers

	permutationLayers := record.getEmptyLayers()
	permutationLayers[0] = traversalLayers[0]

	for layerIndex := 1; layerIndex < len(traversalLayers); layerIndex++ {
		layer := traversalLayers[layerIndex]
		upperLayerSize := record.groups[layerIndex-1]
		prevPositionPermutations := 0
		isPrevPositionHotpoint := false

		for i, validPositionsCount := range layer.mask {
			if validPositionsCount <= 0 {
				continue
			}

			x := i - upperLayerSize - 1
			if x < 0 {
				continue
			}

			if traversalLayers[layerIndex-1].mask[x] < 0 {
				x += traversalLayers[layerIndex-1].mask[x]
			}

			var upperPermutations int
			hotpointVal := hotpointLayers[layerIndex-1].mask[x]
			if hotpointVal != 0 && layerIndex == 1 {
				if hotpointVal < 0 {
					x += hotpointVal
				}
				upperPermutations = hotpointLayers[layerIndex-1].mask[x]
			} else {
				upperPermutations = permutationLayers[layerIndex-1].mask[x]
			}

			permutations := upperPermutations
			isPositionHotpoint := layers[layerIndex].mask[i] < 0

			if !isPositionHotpoint || isPrevPositionHotpoint {
				permutations += prevPositionPermutations
			}
			isPrevPositionHotpoint = isPositionHotpoint
			prevPositionPermutations = permutations
			permutationLayers[layerIndex].mask[i] = permutations
			possibleCombinations = permutations
		}
	}

	if log {
		printLayers(layers)
		// printLayers(validLocationLayers)
		printLayers(traversalLayers)
		printLayers(permutationLayers)
		printLayers(hotpointLayers)
		fmt.Println(hotpoints)
		fmt.Println(mandatoryHotpoints)
	}

	// lastLayer := validLocationLayers[len(validLocationLayers)-1]
	// for i:=lastLayer.start; i < len(lastLayer.mask); i++ {
	//     val := combinationNumberLayers[len(combinationNumberLayers)-1].mask[i]
	//     combinations := val
	//     if val == 0 { continue }

	//     for layerIndex:=len(validLocationLayers)-2; layerIndex>=1; layerIndex-- {
	//         startX := validPositionsByLayer[layerIndex][val]
	//         layerVal := combinationNumberLayers[layerIndex].mask[startX]
	//         intersects := i - startX + record.groups[layerIndex] <= 1
	//         fmt.Println(i, startX, val, layerVal, intersects)
	//         combinations *= layerVal
	//         if intersects {
	//             combinations--
	//         }

	//         possibleCombinations += combinations
	//     }
	// }

	return traversalLayers

	// Merge layers
	// for layerIndex, layer := range layers {
	//     start := -1

	//     for i, val := range layer.mask {
	//         if val == 0 || i == prevStart { continue }

	//         color := layerIndex+1
	//         if i == 0 || colored[i-1] == 0 {
	//             if start == -1 { start = i }
	//             colored[i] = color
	//             continue
	//         }
	//
	//         hasSpaceLeft := i > 1 &&
	//             (colored[i-2] == colored[i-1] || colored[i-2] == 0) &&
	//             colored[i-1] + 1 == color
	//         // hasSpaceRight := i < len(colored) - 2 && colored[i+2] == colored[i+1]
	//         if hasSpaceLeft {
	//             if start == -1 { start = i }
	//             colored[i] = color
	//         }
	//     }

	//     prevStart = start
	// }

	// Ensure monotony
	currentColor := 0
	for i, color := range colored {
		if color < currentColor {
			colored[i] = 0
		} else {
			currentColor = color
		}
	}

	counts := map[int]int{}
	for _, c := range colored {
		if c > 0 {
			counts[c]++
		}
	}

	// Fix conflicts when two groups are adjacent
	// for i:=1; i < len(colored)-1; i++{
	//     if colored[i] == 0 || colored[i-1] == 0 || colored[i-1] == colored[i] { continue }

	//     hasSpaceLeft := i > 1 && colored[i-2] == colored[i-1]
	//     hasSpaceRight := i < len(colored) - 2 && colored[i+2] == colored[i+1]

	//     if !hasSpaceLeft && !hasSpaceRight {
	//         toRemove := i
	//         if counts[colored[toRemove]] == 1 {
	//             toRemove = i-1
	//         }

	//         counts[colored[toRemove]]--
	//         colored[toRemove] = 0
	//     }
	// }

	for _, v := range counts {
		if possibleCombinations == 0 {
			possibleCombinations = v
		} else {
			possibleCombinations *= v
		}
	}

	if log {
		charsStr := make([]string, len(record.chars))
		for i, ch := range record.chars {
			charsStr[i] = string(ch)
		}
		fmt.Println(charsStr, record.groups)
		fmt.Println(colored)
	}

	return traversalLayers
}

func findIslands(record ConditionRecord) Islands {
	pointsOfInterest := []int{}

	for x := 0; x < len(record.chars); x++ {
		char := record.chars[x]

		if char == '#' {
			pointsOfInterest = append(pointsOfInterest, x)
		}
	}

	return pointsOfInterest
}

func getPossibleCombinations(record ConditionRecord) []string {
	// fmt.Println("_________")
	// fmt.Println(string(record.chars), record.groups)

	islands := findIslands(record)

	treeRoot := &Node{level: -1, islands: islands}
	_, combinations := buildTree(record, treeRoot)

	uniqueCombinations := map[string]bool{}

	for _, endNode := range combinations {
		result := endNode.fullResult(record)
		if !uniqueCombinations[result] {
			uniqueCombinations[result] = true
			// endNode.print()
			// fmt.Print(" " + result)
		}
	}

	//fmt.Println("")
	// fmt.Println(len(uniqueCombinations))

	results := make([]string, len(uniqueCombinations))
	i := 0
	for k := range uniqueCombinations {
		results[i] = k
		i++
	}

	return results
}

func buildTree(record ConditionRecord, root *Node) (*Node, []*Node) {
	combinations := []*Node{}

	traversalLayers := getTraversalLayers(record, getValidPositionLayers(record, nil))

	frontier := []*Node{root}
	for len(frontier) > 0 {
		current := frontier[0]
		frontier = frontier[1:]

		if verbose {
			fmt.Println(string(current.chars), current.offset, "lvl:", current.level)
		}

		children := current.findChildren(record, traversalLayers)

		solutions := []*Node{}
		for _, child := range children {
			if verbose {
				fmt.Print(string(child.chars) + "(" + strconv.Itoa(child.offset) + ") > ")
			}
			solutions = append(solutions, child.getPotentialSolutions(record)...)
			if verbose {
				for _, solution := range solutions {
					fmt.Print(string(solution.chars), " ")
				}
				fmt.Println("")
			}
		}

		current.children = solutions
		frontier = append(frontier, solutions...)

		if verbose {
			fmt.Println("\n___________")
		}
		if current.level+1 == len(record.groups)-1 {
			for _, solution := range solutions {
				if len(solution.islands) == 0 {
					combinations = append(combinations, solution)
				}
			}
		}
	}

	return root, combinations
}

func (rootNode *Node) findChildren(record ConditionRecord, traversalLayers []Layer) []*Node {
	if rootNode.level == len(record.groups)-1 {
		return []*Node{}
	}

	//layer := traversalLayers[rootNode.level+1]
	startX := 0
	if rootNode.offset > 0 || len(rootNode.chars) > 0 {
		startX = rootNode.offset + len(rootNode.chars)
	}
	windowSize := record.groups[rootNode.level+1] + 1
	subsegments := []*Node{}

	// for x:=len(layer.mask)-1; x>=startX; {
	//     val := layer.mask[x]
	//     if val < 0 {
	//         x += val
	//         continue
	//     }

	//     segmentChars := []byte{}
	//     if x+windowSize < len(record.chars){
	//         segmentChars = record.chars[x:x+windowSize]
	//     }else{
	//         segmentChars = record.chars[x:]
	//     }
	//
	//     fmt.Println(string(segmentChars))

	//     subsegments = append(
	//         []*Node {
	//             &Node{
	//                 chars: append([]byte{}, segmentChars...),
	//                 level: rootNode.level + 1,
	//                 offset: x,
	//                 previous: rootNode,
	//                 islands: rootNode.islands,
	//             },
	//         },
	//         subsegments...,
	//     )

	//     x--
	// }

	for windowX := startX; windowX < len(record.chars); windowX++ {
		segmentChars := []byte{}
		if windowX+windowSize < len(record.chars) {
			segmentChars = record.chars[windowX : windowX+windowSize]
		} else {
			segmentChars = record.chars[windowX:]
		}

		subsegments = append(subsegments, &Node{
			chars:    append([]byte{}, segmentChars...),
			level:    rootNode.level + 1,
			offset:   windowX,
			previous: rootNode,
			islands:  rootNode.islands,
		})
	}

	return subsegments
}

func (node *Node) getPotentialSolutions(record ConditionRecord) []*Node {
	// Each segment has a responsibility of starting with a . (if offset > 0)
	required := record.groups[node.level]
	solution := []byte{}

	questions := []int{}
	hashtags := []int{}
	dots := []int{}

	if verbose {
		fmt.Println("\nGetting solutions for ", string(node.chars), "(", node.offset, ")")
	}

	mustStartWithDot := false
	var prevLastChar byte
	if node.offset > 0 {
		if node.previous != nil && node.previous.offset+len(node.previous.chars) == node.offset {
			prevLastChar = node.previous.chars[len(node.previous.chars)-1]
		} else {
			prevLastChar = record.chars[node.offset-1]
		}
		mustStartWithDot = prevLastChar != '.'

		if mustStartWithDot && verbose {
			fmt.Println("Must start with dot ", string(prevLastChar))
		}

		if mustStartWithDot && node.chars[0] == '#' {
			if verbose {
				fmt.Println("Must start with dot but can't")
			}

			return []*Node{}
		}
	}

	for i, token := range node.chars {
		switch token {
		case '?':
			questions = append(questions, i)
			solution = append(solution, '#')
		case '#':
			hashtags = append(hashtags, i)
			solution = append(solution, '#')
		case '.':
			dots = append(dots, i)
			solution = append(solution, '.')
		}
	}

	nodes := []*Node{}

	turnFirstQuestionIntoDot := mustStartWithDot && node.chars[0] == '?'
	if turnFirstQuestionIntoDot {
		solution[0] = '.'
		questions = questions[1:]

		if verbose {
			fmt.Println("Turning first question into dot", string(record.chars[node.offset-1]))
		}
	}

	if len(questions)+len(hashtags) < required {
		if verbose {
			fmt.Println("Too little options ", len(questions), "+", len(hashtags), "<", required)
		}
		return nodes
	}

	for _, dotIndex := range dots {
		if dotIndex > 0 && dotIndex < len(node.chars)-1 {
			if verbose {
				fmt.Println("Wrong dot index")
			}
			return nodes
		}
	}

	islands := []int{}
	windowEnd := node.offset + len(node.chars) - 1

	for _, endX := range node.islands {
		if endX < node.offset || endX > windowEnd {
			islands = append(islands, endX)
		}
	}

	// If we can potentially have more than hashtags than needed it can only be 1 more (because of
	// the window size) so we toggle a different ? to . every time to keep the correct number of #s
	if len(questions)+len(hashtags) > required {
		if node.chars[0] == '?' && !turnFirstQuestionIntoDot {
			nodes = append(nodes, &Node{
				chars:    append([]byte{'.'}, solution[1:]...),
				level:    node.level,
				offset:   node.offset,
				previous: node.previous,
				islands:  islands,
			})
		}

		if node.chars[len(node.chars)-1] == '?' {
			nodes = append(nodes, &Node{
				chars:    append(solution[0:len(solution)-1], '.'),
				level:    node.level,
				offset:   node.offset,
				previous: node.previous,
				islands:  islands,
			})
		}
	} else {
		nodes = append(nodes, &Node{
			chars:    solution,
			level:    node.level,
			offset:   node.offset,
			previous: node.previous,
			islands:  islands,
		})
	}

	if verbose {
		fmt.Println("Got ", len(nodes), " solutions", len(questions), len(hashtags), required)
	}
	return nodes
}

func parseConditionRecord(line string) ConditionRecord {
	conditionRecord := ConditionRecord{}
	split := strings.Fields(line)
	conditionRecord.chars = []byte(split[0])
	groupsRaw := strings.Split(split[1], ",")

	for _, group := range groupsRaw {
		groupNumber, _ := strconv.Atoi(group)
		conditionRecord.groups = append(conditionRecord.groups, groupNumber)
	}

	return conditionRecord
}
