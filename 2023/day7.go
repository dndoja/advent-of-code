package main

import (
	"bufio"
	"fmt"
	"sort"
	"strconv"
	"strings"
)

const (
	FiveOfAKind  = 0b111
	FourOfAKind  = 0b110
	FullHouse    = 0b101
	ThreeOfAKind = 0b100
	TwoPair      = 0b011
	OnePair      = 0b010
	HighCard     = 0b001
)

type Hand struct {
	raw   string
	cards [5]byte
	bid   int
	power uint32
}

type Hands []Hand

func (h Hands) Len() int {
	return len(h)
}

func (h Hands) Less(i, j int) bool {
	return h[i].power < h[j].power
}

func (h Hands) Swap(i, j int) {
	h[i], h[j] = h[j], h[i]
}

func RunDay7(scanner *bufio.Scanner) {
	var hands Hands
	var handsWithJokers Hands

	for scanner.Scan() {
		hand := parseHand(scanner.Text())
		hand.power = getHandPower(hand, false)
		hands = append(hands, hand)

		handWithJoker := Hand{
			hand.raw,
			hand.cards,
			hand.bid,
			0,
		}
		handWithJoker.power = getHandPower(hand, true)
		handsWithJokers = append(handsWithJokers, handWithJoker)
	}

	winnings := 0
	winningsWithJokers := 0

	sort.Sort(hands)
	sort.Sort(handsWithJokers)

	for rank := 0; rank < len(hands); rank++ {
		// fmt.Printf("%023b", hands[rank].power)
		// fmt.Print(" ")
		// fmt.Print(hands[rank].raw)
		// fmt.Println("")

		winnings += hands[rank].bid * (rank + 1)
		winningsWithJokers += handsWithJokers[rank].bid * (rank + 1)
	}

	fmt.Println(winnings)
	fmt.Println(winningsWithJokers)
}

// first 3 bits represent the type, 20 others represent individual cards (4 bits each)
// 000 | 0000 0000 0000 0000 0000
func getHandPower(hand Hand, useJokers bool) uint32 {
	cardCounts := make(map[byte]byte)
	combinationCounts := make(map[byte]byte)
	maxCombination := byte(1)
	var weight uint32

	for i, card := range hand.cards {
		if useJokers && card == 11 {
			// Since Joker is the weakest individual card we set its' value to 1
			card = 1
		}

		oldCount := cardCounts[card]
		newCount := oldCount + 1
		cardCounts[card] = newCount

		if card != 1 {
			if combinationCounts[oldCount] > 0 {
				combinationCounts[oldCount]--
			}
			combinationCounts[newCount]++

			if newCount > maxCombination {
				maxCombination = newCount
			}
		}

		weight = weight | (uint32(card) << ((4 - i) * 4))
	}

	var totalCombinations byte
	for combination, count := range combinationCounts {
		if combination > 1 && count > 0 {
			totalCombinations += count
		}
	}

	var handType byte

	switch totalCombinations {
	case 0:
		handType = HighCard
	case 1:
		switch maxCombination {
		case 2:
			handType = OnePair
		case 3:
			handType = ThreeOfAKind
		case 4:
			handType = FourOfAKind
		case 5:
			handType = FiveOfAKind
		}
	case 2:
		if maxCombination == 3 {
			handType = FullHouse
		} else {
			handType = TwoPair
		}
	}

	if useJokers {
		jokersCount := cardCounts[1]

		for i := 0; i < int(jokersCount); i++ {
			if handType == FiveOfAKind {
				break
			}

			if handType >= FullHouse || handType == HighCard {
				handType++
			} else {
				handType += 2
			}
		}
	}

	weight = weight | uint32(handType)<<20

	return weight
}

func parseHand(line string) Hand {
	fields := strings.Fields(line)
	bid, _ := strconv.Atoi(fields[1])
	cards := [5]byte{}

	for i := 0; i < len(cards); i++ {
		char := fields[0][i]

		switch char {
		case 'A':
			cards[i] = 14
		case 'K':
			cards[i] = 13
		case 'Q':
			cards[i] = 12
		case 'J':
			cards[i] = 11
		case 'T':
			cards[i] = 10
		default:
			val, _ := strconv.Atoi(string(char))
			cards[i] = byte(val)
		}
	}

	return Hand{fields[0], cards, bid, 0}
}
