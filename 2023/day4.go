package main

import (
	"bufio"
	"fmt"
	"strconv"
	"strings"
)

type Card struct {
	winningNumbers map[int]bool
	actualNumbers  []int
	multiplier     int
}

func RunDay4(scanner *bufio.Scanner) {
	cards := []Card{}
	pointsSum := 0
	for scanner.Scan() {
		line := scanner.Text()
		card := parseCard(line)
		cards = append(cards, card)
		pointsSum += getCardPoints(card)
	}

	expandWinningCards(cards)

	expandedCardsCount := 0
	for i := 0; i < len(cards); i++ {
		expandedCardsCount += cards[i].multiplier
	}

	fmt.Println(pointsSum)
	fmt.Println(expandedCardsCount)
}

func parseCard(line string) Card {
	numbersRaw := strings.Split(line, ":")[1]
	splitByPipe := strings.Split(numbersRaw, "|")

	winningNumbersRaw := strings.Fields(splitByPipe[0])
	actualNumbersRaw := strings.Fields(splitByPipe[1])

	winningNumbers := map[int]bool{}
	actualNumbers := []int{}

	for i := 0; i < len(winningNumbersRaw); i++ {
		parsed, err := strconv.Atoi(winningNumbersRaw[i])
		if err == nil {
			winningNumbers[parsed] = true
		}
	}

	for i := 0; i < len(actualNumbersRaw); i++ {
		parsed, err := strconv.Atoi(actualNumbersRaw[i])
		if err == nil {
			actualNumbers = append(actualNumbers, parsed)
		}
	}

	return Card{winningNumbers: winningNumbers, actualNumbers: actualNumbers, multiplier: 1}
}

func getCardPoints(card Card) int {
	points := 0

	for i := 0; i < len(card.actualNumbers); i++ {
		number := card.actualNumbers[i]
		if card.winningNumbers[number] {
			if points == 0 {
				points = 1
			} else {
				points = points << 1
			}
		}
	}

	return points
}

func getCardMatchingNumbers(card Card) int {
	matchingNumbers := 0

	for i := 0; i < len(card.actualNumbers); i++ {
		number := card.actualNumbers[i]
		if card.winningNumbers[number] {
			matchingNumbers++
		}
	}

	return matchingNumbers
}

func expandWinningCards(cards []Card) {
	for i := 0; i < len(cards); i++ {
		card := cards[i]
		points := getCardMatchingNumbers(card)

		for offset := 1; offset <= points; offset++ {
			forwardIndex := i + offset
			if forwardIndex >= len(cards) {
				break
			}

			cards[forwardIndex].multiplier += card.multiplier
		}
	}
}
