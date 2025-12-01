package main

import (
	"bufio"
	"fmt"
)

type Node struct {
	leftId  string
	rightId string
}

func RunDay8(scanner *bufio.Scanner) {
	var instructions []byte
	nodes := map[string]Node{}

	scannedFirstLine := false
	for scanner.Scan() {
		line := scanner.Text()

		if !scannedFirstLine {
			instructions = []byte(line)
			scannedFirstLine = true
			continue
		}

		if len(line) == 0 {
			continue
		}

		nodeId, node := parseNode(line)
		nodes[nodeId] = node
	}

	pt1Result := getDistanceToFinish(nodes, instructions, "AAA", "ZZZ")
	fmt.Println(pt1Result)

	// pt.2
	// I cheated on this one by taking a hint.
	// I had completely forgotten about the existence of the LCM as a concept.
	distancesToFinish := []int{}
	for nodeId := range nodes {
		if nodeId[2] == 'A' {
			distance := getDistanceToFinish(nodes, instructions, nodeId, "")
			distancesToFinish = append(distancesToFinish, distance)
		}
	}

	fmt.Println(lcmOfSlice(distancesToFinish))
}

func getDistanceToFinish(
	nodes map[string]Node,
	instructions []byte,
	startingNodeId string,
	finishNodeId string,
) int {
	currentNodeId := startingNodeId
	instructionIndex := 0
	stepCount := 0

	for {
		instruction := instructions[instructionIndex]
		currentNode := nodes[currentNodeId]

		var nextNodeId string
		if instruction == 'L' {
			nextNodeId = currentNode.leftId
		} else {
			nextNodeId = currentNode.rightId
		}

		if instructionIndex+1 < len(instructions) {
			instructionIndex++
		} else {
			instructionIndex = 0
		}

		currentNodeId = nextNodeId
		stepCount++

		if currentNodeId == finishNodeId || (finishNodeId == "" && currentNodeId[2] == 'Z') {
			break
		}
	}

	return stepCount
}

func parseNode(line string) (string, Node) {
	nodeId := line[0:3]
	leftId := line[7:10]
	rightId := line[12:15]

	return nodeId, Node{leftId, rightId}
}

// We have ChatGPT to thank for the following code because I cba to implement the lcm formula
//
// Calculate the Greatest Common Divisor (GCD) using Euclidean algorithm
func gcd(a, b int) int {
	for b != 0 {
		a, b = b, a%b
	}
	return a
}

// Calculate the Least Common Multiple (LCM) of multiple numbers
func lcmOfSlice(numbers []int) int {
	if len(numbers) == 0 {
		return 0
	}

	result := numbers[0]

	for i := 1; i < len(numbers); i++ {
		result = lcm(result, numbers[i])
	}

	return result
}

// Calculate the Least Common Multiple (LCM) of two numbers
func lcm(a, b int) int {
	// LCM(a, b) = |a * b| / GCD(a, b)
	absA := a
	absB := b

	// Avoid division by zero if either a or b is zero
	if absA == 0 || absB == 0 {
		return 0
	}

	// LCM(a, b) = |a * b| / GCD(a, b)
	return absA * absB / gcd(absA, absB)
}
