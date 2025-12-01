package main

import (
	"bufio"
	"fmt"
	"time"
)

const MapSize = 140

type PipesMap = [MapSize][MapSize]byte

const printProcess = false

type Location struct {
	pipe byte
	x    int
	y    int
	next *Location
}

type Point struct {
	x float64
	y float64
}

type Segment struct {
	a Point
	b Point
}

type Line struct {
	slope      float64
	yIntercept float64
	isVertical bool
}

func NewLine(p1 *Point, p2 *Point) Line {
	// y = mx+b
	// m = (y2-y1)/(x2-x1)
	var slope, yIntercept float64
	isVertical := false

	if p2.x == p1.x {
		isVertical = true
	} else {
		slope = (p2.y - p1.y) / (p2.x - p1.x)
		yIntercept = p1.y - slope*p1.x
	}

	return Line{slope, yIntercept, isVertical}
}

func (line *Line) y(x float64) float64 {
	return line.slope*x + line.yIntercept
}

func (point *Point) rayTraceInto(segment Segment) (Point, bool) {
	// Since all of the segments can only form 90 degree angles we trace a ray on a 30 degree
	// angle so it completely avoids running straight into a vertex which is an edge case on
	// the even-odd algorithm
	const rayLength = 20
	const dy = rayLength / 2
	const dx = dy * float64(1.73205080757) // sqrt(3)

	rayPoint := Point{point.x + dx, point.y + dy}

	ray := NewLine(point, &rayPoint)

	segmentLine := NewLine(&segment.a, &segment.b)

	var intX, intY float64
	if segmentLine.isVertical {
		intX = segment.a.x
		intY = ray.y(intX)
	} else {
		intX = (segmentLine.yIntercept - ray.yIntercept) / (ray.slope - segmentLine.slope)
		intY = segmentLine.y(intX)
	}

	var minX, maxX, minY, maxY float64
	if segment.b.x > segment.a.x {
		minX, maxX = segment.a.x, segment.b.x
	} else {
		minX, maxX = segment.b.x, segment.a.x
	}
	if segment.b.y > segment.a.y {
		minY, maxY = segment.a.y, segment.b.y
	} else {
		minY, maxY = segment.b.y, segment.a.y
	}

	intersects := intX > point.x && intX >= minX && intX <= maxX && intY >= minY && intY <= maxY

	return Point{intX, intY}, intersects
}

func RunDay10(scanner *bufio.Scanner) {
	var pipesMap PipesMap
	var animalLocation Location

	y := 0
	for scanner.Scan() {
		line := scanner.Text()
		for x := 0; x < MapSize; x++ {
			pipesMap[y][x] = line[x]

			if line[x] == 'S' {
				animalLocation = Location{pipe: 'S', x: x, y: y}
			}
		}
		y++
	}

	// Find distance for pt1, I could probably unify the logic but for some reason it doesn't work
	// for the same for both parts, so I'm using this flag as a shortcut.
	searchPipesLoop(&animalLocation, pipesMap, true)

	loopStartLocation := searchPipesLoop(&animalLocation, pipesMap, false)
	var loopEndLocation *Location
	var prevInflectionPoint *Location = nil

	currentLocation := loopStartLocation

	polygonSegments := []Segment{}
	visited := map[uint16]bool{currentLocation.id(): true}

	minX, minY := 1000, 1000
	maxX, maxY := 0, 0

	autoPrint := false
	for {
		visited[currentLocation.id()] = true

		if isInflectionPoint(currentLocation.pipe) {
			if prevInflectionPoint == nil {
				prevInflectionPoint = currentLocation
				loopEndLocation = currentLocation
			} else {
				segment := Segment{
					Point{float64(prevInflectionPoint.x), float64(prevInflectionPoint.y)},
					Point{float64(currentLocation.x), float64(currentLocation.y)},
				}

				prevInflectionPoint = currentLocation
				polygonSegments = append(polygonSegments, segment)

				if currentLocation == loopEndLocation {
					break
				}
			}
		}

		if currentLocation.x < minX {
			minX = currentLocation.x
		}
		if currentLocation.x > maxX {
			maxX = currentLocation.x
		}
		if currentLocation.y < minY {
			minY = currentLocation.y
		}
		if currentLocation.y > maxY {
			maxY = currentLocation.y
		}

		if printProcess {
			printMap(pipesMap, currentLocation, visited)
			if !autoPrint {
				var input string
				fmt.Scanln(&input)
				autoPrint = true
			} else {
				time.Sleep(500 * time.Millisecond)
			}
		}

		currentLocation = currentLocation.next
		if currentLocation == nil {
			currentLocation = loopStartLocation
		}
	}

	enclosedTilesCount := 0
	enclosedTiles := map[uint16]bool{}
	checkedCount := 0

	for y := minY; y <= maxY; y++ {
		for x := minX; x <= maxX; x++ {
			if visited[encodeCoords(x, y)] {
				continue
			}

			checkedCount++
			point := Point{float64(x), float64(y)}
			intersectingPoints := []Point{}
			interceptingPolygonSegments := 0

			for _, segment := range polygonSegments {
				interceptionPoint, intercepts := point.rayTraceInto(segment)
				if intercepts {
					intersectingPoints = append(intersectingPoints, interceptionPoint)
					interceptingPolygonSegments++
				}
			}

			if interceptingPolygonSegments%2 != 0 {
				enclosedTiles[encodeCoords(x, y)] = true
				enclosedTilesCount++
			}
		}
	}

	fmt.Println(enclosedTilesCount)
}

func printMap(pipesMap PipesMap, currentLocation *Location, mask map[uint16]bool) {
	// Clear console
	fmt.Print("\033[H\033[2J")

	for y := 0; y < MapSize; y++ {
		line := pipesMap[y]
		for x := 0; x < len(line); x++ {
			shouldShow := mask[encodeCoords(x, y)]
			if currentLocation.x == x && currentLocation.y == y {
				fmt.Print("&")
			} else if shouldShow {
				fmt.Print(string(pipesMap[y][x]))
			} else {
				fmt.Print(".")
			}
		}
		fmt.Println("")
	}
}

func printSegment(pipesMap PipesMap, segment Segment) {
	fmt.Println("")
	if segment.a.y != segment.b.y {
		var minY, maxY float64
		if segment.b.y > segment.a.y {
			minY, maxY = segment.a.y, segment.b.y
		} else {
			maxY, minY = segment.a.y, segment.b.y
		}

		for y := minY; y <= maxY; y++ {
			fmt.Print(string(pipesMap[int(y)][int(segment.a.x)]))
		}
	} else {
		var minX, maxX float64
		if segment.b.x > segment.a.x {
			minX, maxX = segment.a.x, segment.b.x
		} else {
			maxX, minX = segment.a.x, segment.b.x
		}

		for x := minX; x <= maxX; x++ {
			fmt.Print(string(pipesMap[int(segment.a.y)][int(x)]))
		}
	}
	fmt.Print(" ", segment.a, segment.b)
}

func searchPipesLoop(startLocation *Location, pipesMap PipesMap, findMaxDistance bool) *Location {
	frontier := []*Location{startLocation}
	distances := map[uint16]int{startLocation.id(): 0}

	furthestDistance := 0

	for len(frontier) > 0 {
		current := frontier[0]
		frontier = frontier[1:]
		neighbours := current.neighboursIn(pipesMap)

		for _, next := range neighbours {
			nextId := next.id()

			if _, exists := distances[nextId]; !exists {
				frontier = append(frontier, next)
				nextDistance := distances[current.id()] + 1
				distances[nextId] = nextDistance
				current.next = next

				if !findMaxDistance {
					break
				}

				if nextDistance > furthestDistance {
					furthestDistance = nextDistance
				}
			}
		}
	}

	if findMaxDistance {
		fmt.Printf("Furthest distance from entrance: %d \n", furthestDistance)
	}

	return startLocation
}

func (location *Location) id() uint16 {
	return uint16(encodeCoords(location.x, location.y))
}

func encodeCoords(x int, y int) uint16 {
	var encoded uint16
	encoded = uint16(x) << 8
	encoded = encoded | uint16(y)

	return encoded
}

func (location *Location) neighboursIn(pipesMap PipesMap) []*Location {
	x := location.x
	y := location.y
	neighbours := []*Location{}
	north := locationAtCoordinates(pipesMap, x, y-1)
	east := locationAtCoordinates(pipesMap, x+1, y)
	south := locationAtCoordinates(pipesMap, x, y+1)
	west := locationAtCoordinates(pipesMap, x-1, y)

	if north != nil && fitsNorth(location.pipe) && fitsSouth(north.pipe) {
		neighbours = append(neighbours, north)
	}

	if south != nil && fitsSouth(location.pipe) && fitsNorth(south.pipe) {
		neighbours = append(neighbours, south)
	}

	if west != nil && fitsWest(location.pipe) && fitsEast(west.pipe) {
		neighbours = append(neighbours, west)
	}

	if east != nil && fitsEast(location.pipe) && fitsWest(east.pipe) {
		neighbours = append(neighbours, east)
	}

	return neighbours
}

func isInflectionPoint(pipe byte) bool {
	return pipe != '-' && pipe != '|' // && pipe != 'S'
}

func fitsWest(pipe byte) bool {
	return pipe == 'S' || pipe == '7' || pipe == '-' || pipe == 'J'
}

func fitsEast(pipe byte) bool {
	return pipe == 'S' || pipe == '-' || pipe == 'F' || pipe == 'L'
}

func fitsNorth(pipe byte) bool {
	return pipe == 'S' || pipe == '|' || pipe == 'J' || pipe == 'L'
}

func fitsSouth(pipe byte) bool {
	return pipe == 'S' || pipe == '|' || pipe == 'F' || pipe == '7'
}

func locationAtCoordinates(pipesMap PipesMap, x int, y int) *Location {
	if x < 0 || x >= MapSize || y < 0 || y >= MapSize {
		return nil
	}

	return &Location{pipesMap[y][x], x, y, nil}
}
