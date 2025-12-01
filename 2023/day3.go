package main

import (
	"bufio"
	"errors"
	"fmt"
	"strconv"
)

const MATRIX_HEIGHT = 140

type Matrix [MATRIX_HEIGHT][]byte

type Solution struct {
	partsByCoordinates map[uint16]int
	gearRatios         []int
}

func RunDay3(scanner *bufio.Scanner) {
	var matrix Matrix

	row := 0
	for scanner.Scan() {
		line := scanner.Text()
		matrix[row] = []byte(line)
		row++
	}

	solution := getSolution(matrix)

	partsSum := 0
	for _, value := range solution.partsByCoordinates {
		partsSum += value
	}

	gearRatiosSum := 0
	for i := 0; i < len(solution.gearRatios); i++ {
		gearRatiosSum += solution.gearRatios[i]
	}

	fmt.Println(partsSum)
	fmt.Println(gearRatiosSum)
}

func getSolution(matrix Matrix) Solution {
	parts := map[uint16]int{}
	gearRatios := []int{}

	for y := 0; y < len(matrix); y++ {
		for x := 0; x < len(matrix[y]); x++ {
			char := matrix[y][x]
			adjacentParts := map[uint16]int{}

			if char == '.' || (char >= '0' && char <= '9') {
				continue
			}

			for dx := -1; dx <= 1; dx++ {
				for dy := -1; dy <= 1; dy++ {
					if dx == 0 && dy == 0 {
						continue
					}

					coords, nr, err := getNumberAtCoords(matrix, x+dx, y+dy)
					if err == nil {
						adjacentParts[coords] = nr
						parts[coords] = nr
					}
				}
			}

			if char == '*' && len(adjacentParts) == 2 {
				gearRatio := 1

				for _, value := range adjacentParts {
					gearRatio *= value
				}

				gearRatios = append(gearRatios, gearRatio)
			}
		}
	}

	return Solution{partsByCoordinates: parts, gearRatios: gearRatios}
}

func getNumberAtCoords(matrix Matrix, x int, y int) (coords uint16, nr int, err error) {
	if y < 0 || y >= len(matrix) || x < 0 || x >= len(matrix[y]) {
		return 0, 0, errors.New("Coordinates out of bounds")
	}

	char := matrix[y][x]

	if char < '0' || char > '9' {
		return 0, 0, errors.New("Not found")
	}

	numberStr := string(char)
	addedLeft := 0
	stoppedLeft := false
	stoppedRight := false

	for i := 1; !(stoppedLeft && stoppedRight); i++ {
		if !stoppedLeft {
			xLeft := x - i
			if xLeft >= 0 {
				charLeft := matrix[y][xLeft]
				if charLeft >= '0' && charLeft <= '9' {
					numberStr = string(charLeft) + numberStr
					addedLeft++
				} else {
					stoppedLeft = true
				}
			} else {
				stoppedLeft = true
			}
		}

		if !stoppedRight {
			xRight := x + i
			if xRight < len(matrix[y]) {
				charRight := matrix[y][xRight]
				if charRight >= '0' && charRight <= '9' {
					numberStr = numberStr + string(charRight)
				} else {
					stoppedRight = true
				}
			} else {
				stoppedRight = true
			}
		}
	}

	number, err := strconv.Atoi(numberStr)
	numberStartX := x - addedLeft
	coords = (uint16(numberStartX) << 8) + uint16(y)

	return coords, number, nil
}
