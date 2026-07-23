#ifndef ESCAPEROOM_H
#define ESCAPEROOM_H

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Exception class to handle runtime errors gracefully.
// Inherits from std::runtime_error so we can use standard what() method.
class GameException : public std::runtime_error {
public:
  explicit GameException(const std::string &msg) : std::runtime_error(msg) {}
};

// ITEM
// Class for items in the game
class Item {
private:
  std::string id;
  std::string name;

public:
  Item(std::string id, std::string name);
  virtual ~Item() = default;
  std::string getId() const;
  std::string getName() const;
};

// This is the parent class for all interactive room objects
class Interactable {
protected:
  std::string id;
  std::string name;

public:
  Interactable(std::string id, std::string name);
  virtual ~Interactable() = default;

  std::string getId() const;
  std::string getName() const;

  // Pure virtual function for runtime polymorphism [cite: 72]
  virtual std::string examine() = 0;
};

// --- DERIVED CLASS 1: THE RESEARCH DESK ---
class Desk : public Interactable {
public:
  Desk();
  std::string examine() override;
};

// DERIVED INTERACTABLE 2: LOCKED CABINET DRAWER
// Player need the key to open
// Added a state flag to handle progression gating (needs a key to flip
// isLocked).

class Cabinet : public Interactable {
private:
  bool isLocked;

public:
  Cabinet();
  bool getLockedStatus() const;
  void unlock();
  std::string examine() override;
};

// --- DERIVED CLASS 3: UTILITY TOOLBOX ---
// Refactored: Now acts as a physical container tracking concrete Item instances
// [cite: 44, 49]
class Toolbox : public Interactable {
private:
  std::vector<std::shared_ptr<Item>>
      containedItems; // STL Vector of Smart Pointers [cite: 49, 74]
public:
  Toolbox();
  std::string examine() override;

  // Container Management Methods
  void addContainedItem(std::shared_ptr<Item> item);
  bool hasContainedItem(const std::string &itemId) const;
  std::shared_ptr<Item> removeContainedItem(const std::string &itemId);
};

// --- DERIVED CLASS 4: COMPUTER MAINFRAME TERMINAL ---
class Terminal : public Interactable {
public:
  Terminal();
  std::string examine() override;
  bool validatePassword(const std::string &input);
};

//  DERIVED INTERACTABLE 5: EMERGENCY EXIT BLAST DOOR
// If the player gets the killer right, dorr unlock
class ExitDoor : public Interactable {
private:
  bool isDoorUnlocked; // Encapsulated state tracker
public:
  ExitDoor();

  // Polymorphic override for basic inspection
  std::string examine() override;

  // NEW: Concrete method to encapsulate the validation logic inside the class
  bool verifyAccusation(const std::string &suspectName);
  bool getDoorStatus() const;
};

// --- MAIN GAME CONTROLLER ---
class Game {
private:
  std::map<std::string, std::unique_ptr<Interactable>>
      roomObjects; // Hash map container [cite: 74]
  std::vector<std::shared_ptr<Item>>
      inventory; // Player inventory list [cite: 74]
  std::string lastMessage;

public:
  Game();
  void loadInitialState(const std::string &savePath);
  void saveCurrentState(const std::string &savePath);

  std::string executeInspection(const std::string &objectId);
  Toolbox *getToolboxPointer(); // Safe helper to access toolbox container
                                // properties from GUI

  void addItemToInventory(std::shared_ptr<Item> item);
  bool hasItem(const std::string &id) const;
  std::vector<std::string> getInventoryDisplay() const;
  std::string getMessage() const;
  void setMessage(const std::string &msg);
};

#endif // ESCAPEROOM_H
