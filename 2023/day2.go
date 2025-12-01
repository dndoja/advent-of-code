package main

import (
	"bufio"
	"fmt"
)
import "strconv"
import "strings"

type Round struct {
	reds   int
	greens int
	blues  int
}

const (
	MAX_RED_CUBES   = 12
	MAX_GREEN_CUBES = 13
	MAX_BLUE_CUBES  = 14
)

func RunDay2(scanner *bufio.Scanner) {
	gameId := 1
	sum := 0
	powerSum := 0

	for scanner.Scan() {
		line := scanner.Text()
		rounds := parseRounds(line)
		minCubes := getMinRequiredCubes(rounds)

		if minCubes.greens <= MAX_GREEN_CUBES &&
			minCubes.reds <= MAX_RED_CUBES &&
			minCubes.blues <= MAX_BLUE_CUBES {
			sum += gameId
		}
		power := minCubes.blues * minCubes.reds * minCubes.greens
		powerSum += power
		gameId++
	}

	fmt.Println(sum)
	fmt.Println(powerSum)
}

func getMinRequiredCubes(rounds []Round) Round {
	var minCubes Round

	for i := 0; i < len(rounds); i++ {
		round := rounds[i]

		if round.reds > minCubes.reds {
			minCubes.reds = round.reds
		}

		if round.blues > minCubes.blues {
			minCubes.blues = round.blues
		}

		if round.greens > minCubes.greens {
			minCubes.greens = round.greens
		}
	}

	return minCubes
}

func parseRounds(line string) []Round {
	gameRaw := strings.Split(line, ":")[1]
	roundsRaw := strings.Split(gameRaw, ";")
	var rounds []Round

	for i := 0; i < len(roundsRaw); i++ {
		roundRaw := roundsRaw[i]
		cubesRaw := strings.Split(roundRaw, ",")

		var reds int
		var blues int
		var greens int

		for c := 0; c < len(cubesRaw); c++ {
			cubeRaw := cubesRaw[c]
			elements := strings.Fields(cubeRaw)

			count, err := strconv.Atoi(elements[0])
			if err != nil {
				continue
			}

			switch elements[1] {
			case "green":
				greens = count
			case "red":
				reds = count
			case "blue":
				blues = count
			}
		}
		rounds = append(rounds, Round{reds: reds, greens: greens, blues: blues})
	}

	return rounds
}
