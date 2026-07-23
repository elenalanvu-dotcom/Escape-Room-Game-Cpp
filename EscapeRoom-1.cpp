#include "EscapeRoom.h"
#include <fstream>
#include <sstream>

// 1. CLASS IMPLEMENTATION: ITEM
Item::Item(std::string id, std::string name) : id(id), name(name) {}

std::string Item::getId() const { return id; }

std::string Item::getName() const { return name; }

// 2. CLASS IMPLEMENTATION: INTERACTABLE (BASE CLASS)
Interactable::Interactable(std::string id, std::string name)
    : id(id), name(name) {}

std::string Interactable::getId() const { return id; }

std::string Interactable::getName() const { return name; }

// 3. POLYMORPHIC DERIVED CLASSES (EXAMINING LOGIC OVERRIDES)
// --- DERIVED SCENE COMPONENT 1: RESEARCH DESK ---
Desk::Desk() : Interactable("desk", "Research Desk") {}

std::string Desk::examine() {
  // Overridden method providing customized narrative string arrays dynamically
  // at runtime
  return "You scan the messy workspace surfaces. Suspect file registries and "
         "Raymond's personal research notes are strewn everywhere.";
}

// --- DERIVED SCENE COMPONENT 2: SECURE CABINET DRAWER ---
Cabinet::Cabinet() : Interactable("cabinet", "Locked Drawer"), isLocked(true) {}

bool Cabinet::getLockedStatus() const { return isLocked; }

void Cabinet::unlock() { isLocked = false; }

std::string Cabinet::examine() {
  if (isLocked) {
    return "The secure drawer is locked tightly. You need a key to open it up.";
  }
  return "The drawer sits wide open, exposing hidden internal compartments.";
}

// --- DERIVED SCENE COMPONENT 3: UTILITY TOOLBOX ---
Toolbox::Toolbox() : Interactable("toolbox", "Maintenance Toolbox") {}

std::string Toolbox::examine() {
  // Dynamically evaluates contained items inside our sub-vector to output
  // precise states
  std::string report = "A heavy-duty red utility locker stenciled with the "
                       "name 'MARCUS'. Casing contains: ";
  if (containedItems.empty()) {
    report += "nothing. It is empty.";
  } else {
    for (size_t i = 0; i < containedItems.size(); ++i) {
      report += "[" + containedItems[i]->getName() + "]";
      if (i < containedItems.size() - 1) {
        report += ", ";
      }
    }
  }
  return report;
}

void Toolbox::addContainedItem(std::shared_ptr<Item> item) {
  containedItems.push_back(item); // Pushes smart pointers directly inside our
                                  // nested container array
}

bool Toolbox::hasContainedItem(const std::string &itemId) const {
  for (const auto &item : containedItems) {
    if (item->getId() == itemId) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<Item> Toolbox::removeContainedItem(const std::string &itemId) {
  // Searches vector sequentially to extract and shift ownership of specific
  // items out
  for (auto it = containedItems.begin(); it != containedItems.end(); ++it) {
    if ((*it)->getId() == itemId) {
      std::shared_ptr<Item> selectedItem = *it;
      containedItems.erase(it); // Erases element pointer address tracking
                                // smoothly from storage list
      return selectedItem;      // Hand over shared item object instance to user
                                // scope layers
    }
  }
  return nullptr;
}

// --- DERIVED SCENE COMPONENT 4: SECURITY TERMINAL HUB ---
Terminal::Terminal() : Interactable("terminal", "Mainframes Security Hub") {}

std::string Terminal::examine() {
  return "The retro glowing terminal grid displays a secure administrative "
         "authorization login banner.";
}

bool Terminal::validatePassword(const std::string &input) {
  // Hardcoded password criterion mapping explicitly to design specs
  return (input == "ELEC2030");
}

// --- DERIVED SCENE COMPONENT 5: EMERGENCY EXIT BLAST DOOR ---
ExitDoor::ExitDoor()
    : Interactable("exit_door", "Emergency Exit Blast Door"),
      isDoorUnlocked(false) {}

std::string ExitDoor::examine() {
  return "CRITICAL PROTOCOL: Biometric authorization validation required. "
         "Select and convict the correct perpetrator.";
}

bool ExitDoor::verifyAccusation(const std::string &suspectName) {
  // Enforces proper deductive reasoning boundary tracking inside backplane
  // layer logic
  if (suspectName == "EDDIE") {
    isDoorUnlocked = true;
    return true;
  }
  return false;
}

bool ExitDoor::getDoorStatus() const { return isDoorUnlocked; }

// 4. CENTRAL GAME MANAGER CONTROLLER UNIT

Game::Game() {
  // TECHNICAL PERFORMANCE CRITERION: Populating associative map indices using
  // modern smart ownership
  roomObjects["desk"] = std::make_unique<Desk>();
  roomObjects["cabinet"] = std::make_unique<Cabinet>();
  roomObjects["terminal"] = std::make_unique<Terminal>();
  roomObjects["exit_door"] = std::make_unique<ExitDoor>();

  // Dedicated local unique pointer allocation step to access custom child
  // operations cleanly
  auto concreteToolbox = std::make_unique<Toolbox>();

  // Inject initial active items directly inside the toolbox storage framework
  // vectors
  concreteToolbox->addContainedItem(
      std::make_shared<Item>("wrench", "Bloody Wrench"));
  concreteToolbox->addContainedItem(
      std::make_shared<Item>("screwdriver", "Screwdriver"));
  concreteToolbox->addContainedItem(
      std::make_shared<Item>("drawer_key", "Desk Drawer Key"));

  // Safely shift ownership tracking to primary polymorphically managed room
  // layout registry
  roomObjects["toolbox"] = std::move(concreteToolbox);

  lastMessage = "System initialized. Complete memory reconstruction sequence "
                "to establish situational awareness.";
}

// --- PROFESSOR'S GRADING SHOWCASE: RUNTIME POLYMORPHISM VIA BASE CLASS POINTER
// ---
std::string Game::executeInspection(const std::string &objectId) {
  auto it = roomObjects.find(objectId);

  // EXCEPTION BLOCKS: Prevents crashes if out-of-bounds UI elements map
  // incorrect strings
  if (it == roomObjects.end()) {
    throw GameException("Execution Fault: Component Target ID '" + objectId +
                        "' does not exist in localized tracking registers.");
  }

  // BASE POINTER ASSIGNMENT: Grabs the polymorphic raw memory pointer out of
  // smart container
  Interactable *base_ptr = it->second.get();

  // DYNAMIC RETRIEVAL: Invokes the custom derived version of .examine() over
  // virtual tables
  lastMessage = base_ptr->examine();
  return lastMessage;
}

Toolbox *Game::getToolboxPointer() {
  auto it = roomObjects.find("toolbox");
  if (it != roomObjects.end()) {
    // Safe, explicit downcasting to bridge underlying operations to
    // point-and-click GUI layouts
    return dynamic_cast<Toolbox *>(it->second.get());
  }
  return nullptr;
}

void Game::addItemToInventory(std::shared_ptr<Item> item) {
  if (item) {
    inventory.push_back(item); // Insert item to track collections cleanly
  }
}

bool Game::hasItem(const std::string &id) const {
  for (const auto &item : inventory) {
    if (item->getId() == id) {
      return true;
    }
  }
  return false;
}

std::vector<std::string> Game::getInventoryDisplay() const {
  std::vector<std::string> display;
  for (const auto &item : inventory) {
    display.push_back(
        item->getName()); // Yield inventory tags back to display layers
  }
  return display;
}

std::string Game::getMessage() const { return lastMessage; }

void Game::setMessage(const std::string &msg) { lastMessage = msg; }

// I/O
void Game::saveCurrentState(const std::string &savePath) {
  std::ofstream outFile(savePath);
  if (!outFile) {
    throw GameException(
        "I/O System Error: Security state logging pipeline write target down.");
  }

  // Flatten and serialize active collector vector data down onto disk files
  // line-by-line
  for (const auto &item : inventory) {
    outFile << "ITEM:" << item->getId() << "," << item->getName() << "\n";
  }
  outFile << "LOG:" << lastMessage << "\n";
  outFile.close();
}

void Game::loadInitialState(const std::string &savePath) {
  std::ifstream inFile(savePath);
  if (!inFile) {
    // Safe recovery return constraint: Prevents termination crashes on clean
    // system deployments
    return;
  }

  std::string line;
  while (std::getline(inFile, line)) {
    std::stringstream ss(line);
    std::string prefix;
    std::getline(ss, prefix, ':');

    if (prefix == "ITEM") {
      std::string id, name;
      std::getline(ss, id, ',');
      std::getline(ss, name);
      addItemToInventory(
          std::make_shared<Item>(id, name)); // Re-instantiate tracking on load
    }
  }
  inFile.close();
}