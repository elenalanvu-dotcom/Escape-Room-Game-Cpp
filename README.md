# Detective Escape Room 🕵️

A graphical detective escape room game built in **C++** using **GTK**, developed as the final project for Computer System Programming.

Players investigate a mysterious crime scene by exploring interactive objects, collecting evidence, solving puzzles, and identifying the correct suspect to escape the room.

---

## Demo

*(Add screenshots or GIFs here)*

| Main Room | Inventory | Terminal |
|-----------|-----------|----------|
| ![](images/main.png) | ![](images/inventory.png) | ![](images/terminal.png) |

---

# Features

- Interactive point-and-click GUI
- Detective-style mystery gameplay
- Inventory management system
- Object-oriented architecture
- Puzzle progression system
- Terminal authentication puzzle
- Save & Load game functionality
- Exception handling
- Dynamic memory management using smart pointers
- Modular C++ project structure

---

# Technologies

- C++
- GTK
- STL Containers
- Object-Oriented Programming
- Smart Pointers (`std::unique_ptr`, `std::shared_ptr`)
- File I/O
- Event-driven Programming

---

# Project Structure

```
.
├── main.cpp
├── EscapeRoom.cpp
├── EscapeRoom.h
├── assets/
├── saves/
└── README.md
```

---

# Gameplay

The player wakes up inside a locked room after a murder has taken place.

To escape, players must:

- Inspect objects around the room
- Search for hidden clues
- Collect important evidence
- Unlock new interactions
- Access a secured computer terminal
- Identify the real murderer
- Escape successfully

The game is built around an event-driven interaction system where every object behaves independently through polymorphism.

---

# Software Design

The project demonstrates several core Computer System Programming concepts:

## Object-Oriented Programming

- Inheritance
- Encapsulation
- Runtime Polymorphism
- Abstract Base Classes

Example hierarchy:

```
Interactable
├── Desk
├── Cabinet
├── Toolbox
├── Terminal
└── ExitDoor
```

---

## Smart Memory Management

The project uses modern C++ smart pointers.

```cpp
std::unique_ptr<Interactable>
std::shared_ptr<Item>
```

This avoids memory leaks while simplifying ownership management.

---

## STL Containers

- `std::map`
- `std::vector`
- `std::string`

---

## File I/O

The game supports persistent save files by storing:

- Inventory
- Player progress
- Latest game state

---

## Exception Handling

Custom exceptions protect the game against invalid interactions and file errors.

```cpp
class GameException : public std::runtime_error
```

---

# Learning Outcomes

Through this project I gained practical experience with:

- Designing modular C++ applications
- Building event-driven GUI applications
- Applying object-oriented design principles
- Managing dynamic memory safely
- Implementing file persistence
- Handling runtime exceptions
- Organizing large C++ projects

---

# Future Improvements

- Multiple rooms
- Additional puzzles
- NPC dialogue system
- Sound effects
- Animated transitions
- Save slots
- Cross-platform support
- Better UI styling

---

# Author

**Vu Hoang Lan**

Electrical & Computer Engineering @ VinUniversity

Interested in:

- Systems Programming
- Embedded Systems
- FPGA Design
- Software Engineering
- Robotics

LinkedIn:
*(Add your LinkedIn link)*

---

## License

This project was developed for educational purposes.
