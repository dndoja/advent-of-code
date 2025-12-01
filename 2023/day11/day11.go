package day11

import (
	"bufio"
	"fmt"
)

const UniverseSize = 140

type EncodedCoords = uint16
type Universe = [UniverseSize][UniverseSize]byte
type TravelMap = [UniverseSize][UniverseSize]int

func encodeCoords(x int, y int) EncodedCoords {
    return (uint16(x) << 8) | uint16(y)
}

func decodeCoords(encoded EncodedCoords) (int, int) {
    x := (encoded >> 8) & 0xff 
    y := encoded & 0xff

    return int(x), int(y)
}

type Galaxies struct {
    all []EncodedCoords
    byColumns [UniverseSize]bool
    byRows [UniverseSize]bool
}

func (galaxies *Galaxies) getExpansionMultiplierAtPoint(x int, y int) int {
    multiplier := 1

    if !galaxies.byRows[y] {
        multiplier += 1000000 - 1
    }

    if !galaxies.byColumns[x] {
        multiplier += 1000000 - 1
    }

    return multiplier
}

func Run(scanner *bufio.Scanner) {
    galaxies := Galaxies{}
    universe := Universe{}

    y := 0
    for scanner.Scan() {
        line := scanner.Text()
        
        for x:=0;x<UniverseSize; x++ {
            char := line[x]
            if char == '#' {
                galaxies.all = append(galaxies.all, encodeCoords(x,y))
                galaxies.byColumns[x] = true
                galaxies.byRows[y] = true
            }

            universe[y][x] = char
        }
        y++
    }

    distancesSum := 0

    for i, galaxy := range galaxies.all {
        travelMap := getUniverseTravelMap(universe, galaxies, galaxy)
        for otherI, otherGalaxy := range galaxies.all {
            if i == otherI { continue }
            
            x,y := decodeCoords(otherGalaxy)
            dist := travelMap[y][x]
            distancesSum += dist
        }
    }

    fmt.Println(len(galaxies.all))
    fmt.Println(distancesSum / 2)
}

func printMap(travelMap TravelMap) {
    for y:=0;y<UniverseSize;y++{
        for x:=0;x<UniverseSize;x++{
            distance := travelMap[y][x]
            if distance < 10 {
                fmt.Print("  ")
                
            }else{
                fmt.Print(" ")
            }
            fmt.Print(travelMap[y][x])
            fmt.Print("")
        }
        fmt.Println("")
        fmt.Println("")
    }
}

func getUniverseTravelMap(universe Universe, galaxies Galaxies, start EncodedCoords) TravelMap {
    travelMap := TravelMap{}
    frontier := []EncodedCoords{start}
	distances := map[EncodedCoords]int{start: 0}

	for len(frontier) > 0 {
		current := frontier[0]
		frontier = frontier[1:]
        x,y := decodeCoords(current)

        neighbours := getNeighbours(universe, current)

		for _, neighbour := range neighbours {
			if _, exists := distances[neighbour]; !exists {
				frontier = append(frontier, neighbour)
				nextDistance := distances[current] + galaxies.getExpansionMultiplierAtPoint(x,y)
				distances[neighbour] = nextDistance
			}
		}
	}
    
    for coords, distance := range distances {
        x,y := decodeCoords(coords)
        
        travelMap[y][x] = distance
    }

    return travelMap
}

func getNeighbours(universe Universe, center EncodedCoords) []EncodedCoords {
    neighbours := []EncodedCoords {}
    x,y := decodeCoords(center)

    if isPointInUniverse(x-1, y){
        neighbours = append(neighbours, encodeCoords(x-1, y))
    }
    if isPointInUniverse(x+1, y){
        neighbours = append(neighbours, encodeCoords(x+1, y))
    }
    if isPointInUniverse(x, y-1){
        neighbours = append(neighbours, encodeCoords(x, y-1))
    }
    if isPointInUniverse(x, y+1){
        neighbours = append(neighbours, encodeCoords(x, y+1))
    }

    return neighbours
}

func isPointInUniverse(x int, y int) bool {
    return x >= 0 && x < UniverseSize && y >= 0 && y < UniverseSize
}
