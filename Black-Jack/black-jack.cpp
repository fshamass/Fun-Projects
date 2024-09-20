#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

enum class Suites {
  HEART = 0,
  SPADE    ,
  DIAMOND ,
  CLUBS
};

struct Card {
  uint32_t id;
  Suites suite;
  Card(uint32_t num, Suites suite) : id(num), suite(suite) {}
};

class Deck {
  public:
    Deck(uint32_t numCards): numCards_(numCards) {
      cards_.resize(numCards);
    }
    virtual bool deal() {return true;};
    virtual ~Deck() {}
    
    std::pair<std::string,std::string> ConvertSuiteToString(uint32_t id, Suites suite) {
        std::pair<std::string,std::string> ret;
        if(id == 11) {
            ret.first = "JUNIOR";
        } else if(id == 12) {
            ret.first = "QUEEN";
        } else if(id == 13) {
            ret.first = "KING";
        } else {
            ret.first = std::to_string(id);
        }
        switch(suite) {
          case Suites::HEART:
            ret.second = "HEART";
            break;
          case Suites::SPADE:
            ret.second = "SPADE";
            break;
          case Suites::DIAMOND:
            ret.second = "DIAMOND";
            break;
          case Suites::CLUBS:
            ret.second = "CLUBS";
            break;
          default:
            break;
        }
        return ret;
    }
    
    friend std::ostream& operator<<(std::ostream& os, Deck deck) {
      os << "Cards are: \n";
      for(const auto& card:deck.cards_) {
        std::pair<std::string,std::string> cardId = deck.ConvertSuiteToString(card->id, card->suite);
        os << "id: " << cardId.first << " suite: " << cardId.second << std::endl;
      }
      return os;
    }
  protected:
    uint32_t numCards_;
    std::vector<std::shared_ptr<Card>> cards_;
};

class BlackJack : public Deck {
  public:
    BlackJack(): Deck(52), index_(0) {
      for(uint32_t i=0;i<52;i++) {
        uint32_t id = (i%13) +1;
        Suites suite{};
        switch(i/13) {
          case 0:
            suite = Suites::HEART;
            break;
          case 1:
            suite = Suites::SPADE;
            break;
          case 2:
            suite = Suites::DIAMOND;
            break;
          case 3:
            suite = Suites::CLUBS;
            break;
          default:
            break;
        };
        cards_[i] = std::make_shared<Card>(id, suite);
      }
      dealerCards_.clear();
      playerCards_.clear();
    }
    
    void Shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
      std::shuffle(cards_.begin(), cards_.end(),g);
    }
    
    bool deal() override {
      return InitDeal();
    }
    
    bool PlayerHit() {
        bool retValue = false;
        playerCards_.push_back(cards_[index_]);
        std::pair<std::string,std::string> playerCard = ConvertSuiteToString(cards_[index_]->id, cards_[index_]->suite);
        std::cout << "Player gets new card: " << playerCard.first << " " << playerCard.second << std::endl;
        incIndex();
        std::pair<uint32_t,uint32_t> sum = calculateSum(playerCards_);
        std::cout << "PlayerSum first: " << sum.first << " second: " << sum.second << std::endl;
        if((sum.first > 21) && (sum.second > 21)) {
            std::cout << "Player lost ... " << std::endl;
            retValue = true;
            gameEnd();
         } else if((sum.first == 21) || (sum.second == 21)) {
            std::cout << "BlackJack! .. Player Wins .. " << std::endl;
            retValue = true;
            gameEnd();
         }
        return retValue;
    }
    
    bool PlayerStop() {
        bool retValue = false;
        std::pair<uint32_t,uint32_t> playerSum = calculateSum(playerCards_);
        if(playerSum.first != playerSum.second) {
            if(playerSum.second < 21) {
                playerSum.first = playerSum.second;
            }
        }
        std::pair<std::string,std::string> dealerCard = ConvertSuiteToString(dealerCards_[0]->id, dealerCards_[0]->suite);
        std::cout << "Dealer uncover first card: " << dealerCard.first << " " << dealerCard.second << std::endl;
        std::pair<uint32_t,uint32_t> dealerSum;
        bool gameOver = false;
        do {
            dealerSum = calculateSum(dealerCards_);
            if((dealerSum.first > 21) && (dealerSum.second > 21)) {
                std::cout << "Player Wins ... " << std::endl;
                gameEnd();
                retValue = true;
                break;
            } else if((dealerSum.first == 21) || (dealerSum.second == 21)) {
                std::cout << "BlackJack! .. Dealer Wins .." << std::endl;
                gameEnd();
                retValue = true;
                break;
            } else if((dealerSum.second < 21) && (dealerSum.second > playerSum.first)) {
                std::cout << "Dealer Wins ... " << std::endl;
                gameEnd();
                retValue = true;
                break;
            } else if((dealerSum.first < 21) && (dealerSum.first > playerSum.first)) {
                std::cout << "Dealer Wins ... " << std::endl;
                gameEnd();
                retValue = true;
                break;
            } else if((dealerSum.first == playerSum.first) || (dealerSum.second == playerSum.first)) {
                std::cout << "Game Tie ... " << std::endl;
                gameEnd();
                retValue = true;
                break;
            }
            dealerCards_.push_back(cards_[index_]);
            dealerCard = ConvertSuiteToString(cards_[index_]->id, cards_[index_]->suite);
            std::cout << "Dealer pulls new card: " << dealerCard.first << "-" << dealerCard.second << std::endl;
            incIndex();
        } while(gameOver == false);
        return retValue;
    }
    
  private:
    bool InitDeal() {
        bool retValue = false;
        for(int i=0;i<2;i++) {
            dealerCards_.push_back(cards_[index_]);
            incIndex();
        }
        for(int i=0;i<2;i++) {
            playerCards_.push_back(cards_[index_]);
            incIndex();
        }
        std::pair<std::string,std::string> card = ConvertSuiteToString(dealerCards_[1]->id, dealerCards_[1]->suite);
        std::cout << "Dealer gets: " << card.first << "-" << card.second << std::endl;
        card = ConvertSuiteToString(playerCards_[0]->id, playerCards_[0]->suite);
        std::cout << "Player gets: " << card.first << "-" << card.second << "  ";
        card = ConvertSuiteToString(playerCards_[1]->id, playerCards_[1]->suite);
        std::cout << card.first << "-" << card.second << std::endl;
        std::pair<uint32_t,uint32_t> playerSum = calculateSum(playerCards_);
        if(((playerSum.first == 21) || (playerSum.second == 21)) ||
           ((playerCards_[0]->id == 1) && (playerCards_[1]->id == 1))) {
            std::cout << "BalckJack! Player Wins" << std::endl;
            retValue = true;
        }
        return retValue;
    }

    void incIndex() {
      if(index_ < 51) {
        index_++;
      } else {
        Shuffle();
        index_= 0;
      }
    }
    
    std::pair<uint32_t, uint32_t> calculateSum(std::vector<std::shared_ptr<Card>>& cards) {
        uint32_t sum1 = 0;
        uint32_t sum2 = 0;
        for(const auto& card:cards) {
            //std::cout << "From calculateSum - processing Card id: " << card->id << std::endl;
            if(card->id == 1) {
                sum1 += 1;
                sum2 += 11;
            } else if(card->id > 10) {
                sum1 += 10;
                sum2 += 10;
            } else {
                sum1 += card->id;
                sum2 += card->id;
            }
            //std::cout << "From calculateSum: sum1 = " << sum1 << " sum2 = " << sum2 << std::endl;
        }
        return {sum1,sum2};
    }
    
    void gameEnd() {
      dealerCards_.clear();
      playerCards_.clear();
    }

    uint32_t index_;
    std::vector<std::shared_ptr<Card>> dealerCards_;
    std::vector<std::shared_ptr<Card>> playerCards_;
};

int main() {
    BlackJack blackJack;
    blackJack.Shuffle();
    //std::cout << blackJack << std::endl;
    std::string play = "Y";
    bool gameEnd = false;
    std::string cont("y");
    while((play == "Y") || (play == "y")) {
        if(!blackJack.deal()) {
            do {
                std::cout << "Do you wnat to hit? Y-Yes, N-No ";
                 std::cin >> cont;
                 if((cont == "Y") || (cont == "y")) {
                    gameEnd = blackJack.PlayerHit();
                 } else {
                     gameEnd = blackJack.PlayerStop();
                 }
            } while(gameEnd == false);
        }
            
        std::cout << "Do you want to play another game? N-No, Y-Yes " ;
        std::cin >> play;
    }
    std::cout << "Hello, World!";
    return 0;
}

