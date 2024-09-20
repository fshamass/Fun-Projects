# Parking Lot Simulation System
#### Class hierarchy for simulating parking lot which gives the ability to park different vehicle types which require different number of parking spots.  
THe code also give the user the ability to remove vehicle if vehicle moves out of parking lot. Very convenient to be used with Valet parking system

## Design
#### Code consists of the following classes:
#### - Vehicle class: An abstract class to encapsulate the basic feature of vehicle such as License plate, make and model.
#### - Vehicle subclasses (Compact, motorcycle and bus): add the ability to specify how many parking spots this vehicle needs.
#### - Parking Spot class represents single parking spot and it's status
#### - Parking Level class that represents single level of parking lots. It takes number of rows and number of spots per row to be constructed. Each is represented by parking spot class.
#### - Finally Parking Lots class represents the whole parking lots system. It takes number of levels each is represented by parking level class

The code is designed to be scaled easily to accommodate new vehicles are needed or how many parking rows and spots per parking changes without changing the code
Let me know if you have any comments or feedback faiq.shamass@gmail.com
