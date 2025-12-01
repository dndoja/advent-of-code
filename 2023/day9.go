package main

import (
	"bufio"
	"fmt"
	"strconv"
	"strings"
)

func RunDay9(scanner *bufio.Scanner) {
	leftSum := 0
	rightSum := 0

	for scanner.Scan() {
		line := scanner.Text()
		sequence := parseSequence(line)
		leftVal, rightVal := extrapolateValues(sequence)
		leftSum += leftVal
		rightSum += rightVal
	}

	fmt.Println(rightSum)
	fmt.Println(leftSum)
}

func extrapolateValues(sequence []int) (int, int) {
	sequences := [][]int{sequence}

	for {
		prevSequence := sequences[len(sequences)-1]
		differences, isFullOfZeroes := getSequenceDifferences(prevSequence)

		sequences = append(sequences, differences)

		if isFullOfZeroes {
			break
		}
	}

	for i := len(sequences) - 2; i >= 0; i-- {
		bottomSequence := sequences[i+1]
		currentSequence := sequences[i]
		rightVal := currentSequence[len(currentSequence)-1] + bottomSequence[len(bottomSequence)-1]
		leftVal := currentSequence[0] - bottomSequence[0]

		sequences[i] = append([]int{leftVal}, sequences[i]...)
		sequences[i] = append(sequences[i], rightVal)
	}

	return sequences[0][0], sequences[0][len(sequences[0])-1]
}

func getSequenceDifferences(sequence []int) ([]int, bool) {
	differences := []int{}
	isFullOfZeroes := true

	for i := 1; i < len(sequence); i++ {
		difference := sequence[i] - sequence[i-1]
		differences = append(differences, difference)
		if difference != 0 {
			isFullOfZeroes = false
		}
	}

	return differences, isFullOfZeroes
}

func parseSequence(line string) []int {
	fields := strings.Fields(line)
	sequence := []int{}

	for _, field := range fields {
		number, _ := strconv.Atoi(field)
		sequence = append(sequence, number)
	}

	return sequence
}
