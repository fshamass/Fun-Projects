#include <iostream>
#include <string>
#include <memory>
#include <queue>

enum class VehicleType {
  COMPACT = 0,
  MOTORCYCLE,
  BUS
};

enum class Status {
  FREE = 0,
  OCCUPIED
};

class Vehicle {
  std::string licensePlate;
  std::string model;
  std::string make;
  public:
    Vehicle(std::string licPlate, std::string model, std::string make):
      licensePlate(licPlate), model(model), make(make) {}
    virtual ~Vehicle() {}
    std::string getLicensePlate() { return licensePlate;}
    std::string getModel() { return model;}
    std::string getMake() { return make;}
    virtual VehicleType getType() = 0;
    virtual uint32_t getNeededSpots() = 0;
};

class Compact : public Vehicle {
  VehicleType type_;
  uint32_t neededSpots_;
  public:
    Compact(std::string licPlate, std::string model, std::string make):
      type_(VehicleType::COMPACT), neededSpots_(1), Vehicle(licPlate, model, make) {}
    ~Compact() = default;
    VehicleType getType() override { return type_;}
    uint32_t getNeededSpots() override { return neededSpots_;}
};

class MotorCycle : public Vehicle {
  VehicleType type_;
  uint32_t neededSpots_;
  public:
    MotorCycle(std::string licPlate, std::string make, std::string model):
      type_(VehicleType::MOTORCYCLE), neededSpots_(1), Vehicle(licPlate, model, make) {}
    ~MotorCycle() = default;
    VehicleType getType() override { return type_;}
    uint32_t getNeededSpots() override { return neededSpots_;}
};

class Bus : public Vehicle {
  VehicleType type_;
  uint32_t neededSpots_;
  public:
    Bus(std::string licPlate, std::string make, std::string model) :
      type_(VehicleType::BUS), neededSpots_(5), Vehicle(licPlate, make, model) {}
    ~Bus() = default;
    VehicleType getType() override { return type_;}
    uint32_t getNeededSpots() override { return neededSpots_;}
};

class ParkingSpot {
  public:
    ParkingSpot(uint32_t id) : id_(id), vehicle_(nullptr), status_(Status::FREE) {}
    ~ParkingSpot() = default;
    Status getStatus() {return status_;}
    void setStatus(Status status) {status_ = status;}
    void setVehicle(std::shared_ptr<Vehicle> vehicle) { vehicle_ = vehicle;}
    std::shared_ptr<Vehicle> getVehicle() {return vehicle_;}
  private:
    Status status_;
    std::shared_ptr<Vehicle> vehicle_;
    uint32_t id_;
};

class ParkingLevel {
  public:
    ParkingLevel(int numRows, int spotsPerRow) {
      for(int i=0;i<numRows;i++) {
        std::vector<std::shared_ptr<ParkingSpot>> tmp(spotsPerRow, nullptr);
        for(int j=0;j<spotsPerRow;j++) {
          tmp[j] = std::make_shared<ParkingSpot>(i*j);
        }
        spots_.push_back(std::move(tmp));
      }
    }

    std::pair<int,int> parkVehicle(std::shared_ptr<Vehicle> vehicle) {
      int row = -1, spot = -1;
      if(vehicle) {
        uint32_t numSpotsNeeded = vehicle->getNeededSpots();
        for(int i=0;i<spots_.size();i++) {
          auto index = canFitVehicle(numSpotsNeeded, spots_[i]);
          if(index != -1) {
            row = i + 1;
            spot = index + 1;
            for(int k=index;k<(index+numSpotsNeeded);k++) {
              spots_[i][k]->setStatus(Status::OCCUPIED);
              spots_[i][k]->setVehicle(vehicle);
            }
            break;
          }
        }
      }
      return std::make_pair(row,spot);
    }

  bool removeVehicle(int row, int spot) {
    int r = row-1;
    int s = spot -1;
    bool ret = false;
    std::shared_ptr<ParkingSpot> park = spots_[r][s];
    std::shared_ptr<Vehicle> vehicle = park->getVehicle();
    if(vehicle) {
      int numSpots = vehicle->getNeededSpots();
      for(int i=s;i<(numSpots+s);i++) {
        spots_[r][i]->setStatus(Status::FREE);
        spots_[r][i]->setVehicle(nullptr);
      }
      ret = true;
    }
    return ret;
  }

  bool removeVehicle(std::shared_ptr<Vehicle> vehicle) {
    bool ret = false;
    for(int i=0;i<spots_.size();i++) {
      for(int j=0;j<spots_[i].size();j++) {
        auto v = spots_[i][j]->getVehicle();
        if((v != nullptr) && (v->getLicensePlate() == vehicle->getLicensePlate()) &&
           (v->getModel() == vehicle->getModel()) &&
           (v->getMake() == vehicle->getMake())) {
            ret = true;
            removeVehicle(i+1,j+1);
        }
      }
    }
    return ret;
  }

  std::shared_ptr<Vehicle> getVehicleInfo(int row, int spot) {
    int r = row -1;
    int s = spot-1;
    return spots_[r][s]->getVehicle();
  }

  Status getParkingSpotStatus(int row, int spot) {
    int r = row -1;
    int s = spot -1;
    return spots_[r][s]->getStatus();
  }

  friend std::ostream& operator<<(std::ostream& os, ParkingLevel parkingLevel) {
    std::string statStr("");
    std::string licPlateStr("");
    for(uint32_t i=0;i<parkingLevel.spots_.size();i++) {
      for(uint32_t j=0;j<parkingLevel.spots_[i].size();j++) {
        statStr = (parkingLevel.spots_[i][j]->getStatus() == Status::FREE)? "FREE    ":"OCCUPIED";
        licPlateStr = (parkingLevel.spots_[i][j]->getVehicle())?
          parkingLevel.spots_[i][j]->getVehicle()->getLicensePlate():"N/A";
        os << "Row: " << (i+1) << ", Spot: " << (j+1) << " - Status: "
           << statStr << ", License Plate: " << licPlateStr << std::endl;
      }
    }
    return os;
  }

  private:
    int canFitVehicle(uint32_t numSpots, std::vector<std::shared_ptr<ParkingSpot>> row) {
      uint32_t numConsSpots = 0;
      int index = -1;
      int lastValidIndex = -1;
      for(int i=0;i<row.size();i++) {
        if(row[i]->getStatus() == Status::FREE) {
          if(lastValidIndex < 0) {
            lastValidIndex=i;
          }
          numConsSpots++;
          if(numConsSpots >= numSpots) {
            //Increment index by 1 since spots are 1 based and not 0 based
            index = lastValidIndex;
            break;
          }
        } else {
          numConsSpots = 0;
          lastValidIndex = -1;
        }
      }
      return index;
    }
    std::vector<std::vector<std::shared_ptr<ParkingSpot>>> spots_;
};

class ParkingLots {
  public:
    //Pair is for level configuration (number of rows and spot per each rows
    ParkingLots(std::vector<std::pair<uint32_t, uint32_t>> levelConfig) {
      for(uint32_t i = 0; i<levelConfig.size();i++) {
        parkingLots_.push_back(std::make_shared<ParkingLevel>(levelConfig[i].first, levelConfig[i].second));
      }
    }
    ~ParkingLots() {}

    std::tuple<int,int,int> parkVehicle(std::shared_ptr<Vehicle> vehicle) {
      int row = -1, spot = -1, level = -1;
      for(int i=0;i<parkingLots_.size();i++) {
        auto res = parkingLots_[i]->parkVehicle(vehicle);
        if((res.first != -1) && (res.second != -1)) {
          row = res.first;
          spot = res.second;
          level = i+1;
          break;
        }
      }
      return std::make_tuple(level,row,spot);
    }

    bool removeVehicle(std::shared_ptr<Vehicle> vehicle) {
      bool ret = false;
      //Check for each level whther the vehicle exists
      for(int i=0;i<parkingLots_.size();i++) {
        if(parkingLots_[i]->removeVehicle(vehicle)) {
          ret = true;
          break;
        }
      }
      return ret;
    }

    bool removeVehicle(int level, int row, int spot) {
      return (parkingLots_[level-1]->removeVehicle(row, spot));
    }

    std::shared_ptr<Vehicle> getVehicleInfo(int level, int row, int spot) {
     return parkingLots_[level-1]->getVehicleInfo(row,spot);
    }

    Status getParkingSpotStatus(int level, int row, int spot) {
      return parkingLots_[level-1]->getParkingSpotStatus(row,spot);
    }

    friend std::ostream& operator<<(std::ostream& os, ParkingLots parkingLots) {
      os << "****************** Current Parking Lot Status ********************" << std::endl;
      for(uint32_t i = 0; i< parkingLots.parkingLots_.size(); i++) {
        os << "Level: " << (i+1) << ": \n" << *(parkingLots.parkingLots_[i]);
      }
      return os;
    }
  private:
    std::vector<std::shared_ptr<ParkingLevel>> parkingLots_;
};


int main()
{
    std::vector<std::pair<uint32_t,uint32_t>> levels{{2,6},{2,6}};
    ParkingLots* parkingLots = new ParkingLots(levels);
    std::string StatStr("");
    std::string LicPlateStr("");
    auto bus1 = std::make_shared<Bus>("171AZ23", "Totoya", "Tata");
    auto bus2 = std::make_shared<Bus>("AZM2323", "Honda", "Pilot");
    auto bus3 = std::make_shared<Bus>("QVM2111", "Honda", "Pilot");
    auto motorcycle1 = std::make_shared<MotorCycle>("CA21AB1", "Honda", "AX500");
    auto compact1 = std::make_shared<Compact>("WM7AZ89", "Lexus", "IS250");

    auto park1 = parkingLots->parkVehicle(bus1);
    std::cout << "Vehicle: " << bus1->getLicensePlate() << " was parked at level: " <<
      std::get<0>(park1) << " row: " << std::get<1>(park1) << " spot: " << std::get<2>(park1) << std::endl;

    auto park2 = parkingLots->parkVehicle(bus2);
    std::cout << "Vehicle: " << bus2->getLicensePlate() << " was parked at level: " <<
      std::get<0>(park2) << " row: " << std::get<1>(park2) << " spot: " << std::get<2>(park2) << std::endl;

    auto park3 = parkingLots->parkVehicle(bus3);
    std::cout << "Vehicle: " << bus3->getLicensePlate() << " was parked at level: " <<
      std::get<0>(park3) << " row: " << std::get<1>(park3) << " spot: " << std::get<2>(park3) << std::endl;

    auto park4 = parkingLots->parkVehicle(motorcycle1);
    std::cout << "Vehicle: " << motorcycle1->getLicensePlate() << " was parked at level: " <<
      std::get<0>(park4) << " row: " << std::get<1>(park4) << " spot: " << std::get<2>(park4) << std::endl;

    std::cout << *(parkingLots) << std::endl;

    std::cout << "Removing Vehicle: " << bus2->getLicensePlate();
    if(parkingLots->removeVehicle(bus2)) {
    //if(parkingLots->removeVehicle(1,1,1)) {
      std::cout << " succeeded\n";
    } else {
      std:: cout << " failed\n";
    }

    std::cout << "Removing Vehicle at level: 1, row: 1, spot 6 ";
    if(parkingLots->removeVehicle(1,1,6)) {
      std::cout << " succeeded\n";
    } else {
      std:: cout << " failed\n";
    }

    std::cout << *(parkingLots) << std::endl;

    return 0;
}