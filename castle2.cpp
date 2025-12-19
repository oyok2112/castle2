//
//  CASTLE2.CPP
//    by oyok
//
//  see readme for description
//
// todo (future releases probably):
//  spells
//  insight: see hidden things like what keys go to what door, hidden rooms, etc

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;


struct Player {
    string name = "oyok";
    int maxhp = 10;
    int hp = 10;
};

// Item system
struct Item {
    string name;
    string description;
    int attackDmg;
    int throwDmg;
    bool canTake;
    bool isWeapon;
    bool isKey;
};

// Room structure
struct Room {
    string name;
    string description;
    string altText;
    bool exists = false;  // Is there a room here at all?
    bool exitNorth = false;
    bool exitSouth = false;
    bool exitEast = false;
    bool exitWest = false;
    vector<string> items;
    bool visited = false;
    string visitEvent = "none";
    string key = "none";
    string keyEvent = "none";
    bool hidden = false;
    bool altDisp = false;
};


struct Enemy {
    int enemyY;
    int enemyX;
    string name;
    string description;
    int maxhp;
    int hp;
    int atk;
    bool aggro;
    bool defeated;
    string event;
    string defeatedMessage;
};


static void enterPause() {
    string input;
    cout << "\npress enter to continue...";
    getline(cin, input);
}


static void printWrapped(const string& text, int maxWidth = 80) {
    stringstream ss(text);
    string word;
    int currentLineLength = 0;
    
    while (ss >> word) {
        // Check if adding this word would exceed max width
        if (currentLineLength + word.length() + 1 > maxWidth) {
            cout << "\n";  // Start new line
            currentLineLength = 0;
        }
        
        // Add space before word if not at start of line
        if (currentLineLength > 0) {
            cout << " ";
            currentLineLength++;
        }
        
        cout << word;
        currentLineLength += word.length();
    }
    
    cout << "\n";  // Final newline
}


class Game {
private:
    int MAP_WIDTH;
    int MAP_HEIGHT;
    
    bool gameover = false;

    Player player;
    vector<vector<Room>> rooms;
    vector<vector<string>> worldMap;
    int playerX;
    int playerY;
    
    vector<string> inventory;
    vector<string> bestiary;
    int health = 100;
    map<string, Item> itemDatabase;
    map<string, Enemy> enemyDatabase;
    int currentLevel = 1;
    
    //LevelManager levelManager;
    
public:
    Game() {
        initializeItems(currentLevel);
        initializeEnemies(currentLevel);
        for (const auto& [enemyKey, enemyData] : enemyDatabase) {
            bestiary.push_back(enemyKey);
        }
        initializeLevel(currentLevel);
        initializeMap();
    }

    void initializeItems(int levelID) {
        if (levelID == 0)
        {
            itemDatabase["machete"] = {"Machete", "Looks like a brush-cuttin' gate clearin' whackin' machine.", 1, 0, true, true, false};
            itemDatabase["gate key"] = {"Gate Key", "A key that seems to fit the castle gate.",0, 10, true, false, true};
            itemDatabase["dagger"] = {"Dagger", "A small, yet underwhelming, dagger.  I guess size does matter.", 2, 1, true, true, true};
        }
    }
    
    void initializeEnemies(int levelID) {
        if (levelID == 0) {
            enemyDatabase["Brush"] = {2, 2, "Brush", "This overgrowth is entirely out of hand.", 1, 1, 0, false, false, "exitNorth=true", "The brush was cleared!"};
            enemyDatabase["Castle Guard"] = {1, 1, "Castle Guard", "This guard looks like he does not...fuck......around.", 10, 10, 10, true, false, "key=gate key", "The guard was vanquished!"};
        }
    }

    void initializeLevel(int levelID) {
        
        if (levelID == 0) {
            MAP_WIDTH = 4;
            MAP_HEIGHT = 5;
            playerX = 3;
            playerY = 2;
        }
        else if (levelID == 1) {
            MAP_WIDTH = 8;   // or whatever size you want for level 1
            MAP_HEIGHT = 8;  // now level 1 can be bigger!
            playerX = 0;
            playerY = 5;
        }

        // Resize the vectors based on new dimensions
        rooms.resize(MAP_HEIGHT);
        for (int y = 0; y < MAP_HEIGHT; y++) {
            rooms[y].resize(MAP_WIDTH);
            // Clear each room
            for (int x = 0; x < MAP_WIDTH; x++) {
                rooms[y][x] = Room();
            }
        }
        
        // clear map
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                rooms[y][x] = Room();
            }
        }


        if (levelID == 0) {
            playerX = 3;  // Starting position
            playerY = 2;
            // Starting Room at (2, 3)
            rooms[2][3].exists = true;
            rooms[2][3].name = "Lake Teakwood";
            rooms[2][3].description = "You are standing in front of a modestly-sized lake, the sun is getting lower in the sky and casting shadows from the tree line just to your north.  Rithman is to the south and Castle Gilderhinf is supposedly to the north, although the crumbling stone gates are completely covered by overgrowth.";
            rooms[2][3].exitWest = true;
            rooms[2][3].exitSouth = true;

            rooms[3][3].exists = true;
            rooms[3][3].name = "Lake Teakwood - South";
            rooms[3][3].description = "Lake Teakwood is just to your north, and Rithman's chapel steeple is visible on the south horizon beyond a heavily wooded area. Now that you are further south of the treeline, you can clearly see the girthy towers of the overcompensating Gilderhinf's Castle.";
            rooms[3][3].exitWest = true;
            rooms[3][3].exitNorth = true;

            rooms[1][3].exists = true;
            rooms[1][3].name = "Lake Teakwood - North";
            rooms[1][3].description = "You're at the north end of lake Teakwood, and looking further north you see the castle walls looming. The sunset is once again making something shine under the water. This is the last time you will see this trope. But it's a key. You'll never guess what it's used for.";
            rooms[1][3].exitWest = true;
            rooms[1][3].items.push_back("gate key");

            rooms[2][2].exists = true;
            rooms[2][2].name = "Really Thick Brush Here...";
            rooms[2][2].description = "That damn overgrowth I mentioned is particualarly thick here. Like, too thick to clear with your bare hands...but the gate is right there if you could only...hmm...";
            rooms[2][2].altText = "The path is clear, though no eyes can see...you turn off your Classic Genesis (The Gabriel Era) playlist and notice that you have cleared a path through the overgrown gate...";
            rooms[2][2].exitEast = true;

            rooms[4][2].exists = true;
            rooms[4][2].name = "South Rithman Road";
            rooms[4][2].description = "This is the edge of the barren fields to the north, and a path extends southward to Rithman. You just came here after spending the night in Rithman actually. It was somewhat nice but lacked any real charm. Hassibi was a much nicer town really. In fact, Rithman can't even be bothered to have a decent inn with decent food AND ale, you have to get a nice dinner at the Central Tavern and the only good drinks are across town, almost to the fucking castle walls, at the North Ender. I mean I understand that you can't be a jack of all trades but can you at least figure out the basics of what they would teach you in leisure studies 101? Also if you look closely (and you're still reading) you see a small cave opening to your west...";
            rooms[4][2].altText = "You heard me the first time...";
            rooms[4][2].visitEvent = "altDisp=true";
            rooms[4][2].exitNorth = true;
            rooms[4][2].exitWest = true;

            rooms[4][1].exists = true;
            rooms[4][1].name = "Southern Woods";
            rooms[4][1].description = "There is a shiny metallic item buried under some twigs and leaves here, should you take what is obviously a blade of some kind? Does it psyche you out that I am presenting it as a choice?";
            rooms[4][1].altText = "With the leaves cleared, this is a pretty nice clearing to sit and relax for a minute.  Looks like this might have been a bed for a small animal, perhaps a racoon or stray puppy...";
            rooms[4][1].exitEast = true;
            rooms[4][1].items.push_back("machete");
            
            rooms[3][2].exists = true;
            rooms[3][2].name = "Barren Fields";
            rooms[3][2].description = "Not much to see here, these fields look like they once were used to raise expensive livestock or rare herbs, spices, and/or peppers. At least that's what the worn out sign says they sold here by the road.";
            rooms[3][2].exitSouth = true;
            rooms[3][2].exitEast = true;

            rooms[1][1].exists = true;
            rooms[1][1].name = "Paved Castle Road";
            rooms[1][1].description = "Directly north of you, there is a moat and castle gate (typical shit). The gate is probably locked, and you probably shouldn't go there because there is something patrolling the area...or at least it looks like it...yeah...";
            rooms[1][1].exitNorth = true;
            rooms[1][1].exitEast = true;

            rooms[1][2].exists = true;
            rooms[1][2].name = "Wooded Path";
            rooms[1][2].description = "There is a signpost at the tail of a wooded path here, it says [Castle Gilderhinf - Ahead Westwardly] scribbled beneath the printed sign is what appears to read, 'keep out' but it looks like a fucking kid wrote it.";
            rooms[1][2].exitWest = true;
            rooms[1][2].exitEast = true;
            rooms[1][2].hidden = true;

            rooms[0][1].exists = true;
            rooms[0][1].name = "Castle Gate";
            rooms[0][1].description = "You are standing at the castle gate, which is always guarded by a giagantic brute.";
            rooms[0][1].altText = "Easy game for babies. The gate stands before you with the standard issue Gilderhinf-style keyhole hardware.";
            rooms[0][1].exitSouth = true;
            rooms[0][1].key = "gate key";
            rooms[0][1].keyEvent = "nextLevel";
        }
        else if (levelID == 1) {
            playerX = 0;  // Starting position
            playerY = 5;
            // Starting Room at (2, 3)
            rooms[5][0].exists = true;
            rooms[5][0].name = "Shit Got Real";
            rooms[5][0].description = "Castle starts now";
            rooms[5][0].exitSouth = true;
            rooms[5][0].exitEast = true;
            rooms[5][0].exitNorth = true;

            rooms[4][0].exists = true;
            rooms[4][0].name = "North West Foyer";
            rooms[4][0].description = "This is a wide open entryway to the castle, a foyer if you will.";
            rooms[4][0].exitSouth = true;
            rooms[4][0].exitEast = true;

            rooms[6][0].exists = true;
            rooms[6][0].name = "South West Foyer";
            rooms[6][0].description = "An empty part of a really empty looking front hallway, not only empty, but stinky also.";
            rooms[6][0].altText = "I mean for gods' sake it smells like it hasn't been cleaned in here for...well.";
            rooms[6][0].exitNorth = true;
            rooms[6][0].exitEast = true;
            rooms[6][0].visitEvent = "altDisp=true";

            rooms[4][1].exists = true;
            rooms[4][1].name = "North East Foyer";
            rooms[4][1].description = "The hallway narrows and extends eastward, south of here there seems to be a similar hallway. Which hall do I take?";
            rooms[4][1].exitSouth = true;
            rooms[4][1].exitEast = true;
            rooms[4][1].exitWest = true;

            rooms[5][1].exists = true;
            rooms[5][1].name = "East Foyer";
            rooms[5][1].description = "The \"grand\" entryway is west of here, two forking hallways extend eastward to the north and south. Mother, forking, hallways.";
            rooms[5][1].exitNorth = true;
            rooms[5][1].exitSouth = true;
            rooms[5][1].exitWest = true;

            rooms[6][1].exists = true;
            rooms[6][1].name = "South East Foyer";
            rooms[6][1].description = "The corner of the front hall, which upon closer inspection has a hallway, yes, but also a door is locked that would let you into that hallway. To the east. Otherwise there's that other hallway...";
            rooms[6][1].exitNorth = true;
            rooms[6][1].exitWest = true;
            rooms[6][1].key = "Southall Key";
            rooms[6][1].keyEvent = "exitEast=true";

            rooms[2][2].exists = true;
            rooms[2][2].name = "North Hallway - West End";
            rooms[2][2].description = "The end of a long corridor leading east as far as you can see. The torches on the wall seem to cast a shadow about halfway down.";
            rooms[2][2].key = "test";
            rooms[2][2].exitSouth = true;
            rooms[2][2].exitEast = true;

            rooms[3][2].exists = true;
            rooms[3][2].name = "Boring Passage";
            rooms[3][2].description = "This is seriously just filler. Not a hint, filler room, move on.";
            rooms[3][2].altText = "I said, move on...";
            rooms[3][2].visitEvent = "altDisp=true";
            rooms[3][2].exitNorth = true;
            rooms[3][2].exitSouth = true;

            rooms[4][2].exists = true;
            rooms[4][2].name = "South Passage";
            rooms[4][2].description = "From here you can see the end of a long hallway to the north, and you can hear footsteps down the hall opposite the entryway.";
            rooms[4][2].exitNorth = true;
            rooms[4][2].exitEast = true;
            rooms[4][2].exitWest = true;

            rooms[6][2].exists = true;
            rooms[6][2].name = "Closed Hallway";
            rooms[6][2].description = "The hallway here was concealed by a locked door.  Now opened, you can see a dark corner curving around the end of the corridor.";
            rooms[6][2].exitEast = true;
            rooms[6][2].exitWest = true;

        }
    }

    void initializeMap() {
        worldMap.resize(MAP_HEIGHT);
        for (int y = 0; y < MAP_HEIGHT; y++) {
            worldMap[y].resize(MAP_WIDTH);
            for (int x = 0; x < MAP_WIDTH; x++) {
                string mapChar = "-";
                if (rooms[y][x].exists && !rooms[y][x].hidden) {
                    mapChar = ".";
                }
                worldMap[y][x] = mapChar;
            }
        }
    }

    void printMap() {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                // Check player position first (highest priority)
                if (x == playerX && y == playerY) {
                    cout << "@";
                }
                else {
                    bool enemyFound = false;
                    for (const auto& [enemyKey, enemy] : enemyDatabase) {
                        if (enemy.enemyX == x && enemy.enemyY == y && !enemy.defeated) {
                            cout << enemy.name[0];  // First character of enemy name
                            enemyFound = true;
                            break;  // Only show one enemy per tile
                        }
                    }
                    // If no enemy, show the map character (room or empty)
                    if (!enemyFound) {
                        if (rooms[y][x].hidden && rooms[y][x].visited) {
                            cout << ".";
                        } else
                            cout << worldMap[y][x];
                    }
                }

            }
            cout << "\n";
        }
        cout << "\n";
    }
    
    void printRoom() {
        Room& room = rooms[playerY][playerX];
        
        cout << "\n==================================\n";
        cout << room.name << "\n";
        cout << "==================================\n";
        printMap();
        if (!room.altDisp) {
            printWrapped(room.description);
        } else {
            printWrapped(room.altText);
        }
        
        if (!room.items.empty()) {
            cout << "\nYou see: ";
            for (size_t i = 0; i < room.items.size(); i++) {
                cout << itemDatabase[room.items[i]].name;
                if (i < room.items.size() - 1) cout << ", ";
            }
            cout << "\n";
        }
        
        cout << "\nExits: ";
        if (room.exitNorth) cout << "north ";
        if (room.exitSouth) cout << "south ";
        if (room.exitEast) cout << "east ";
        if (room.exitWest) cout << "west ";
        cout << "\n";
        
        room.visited = true;
    }
  
    void processCommand(string input) {
        // Convert to lowercase
        transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        stringstream ss(input);
        string command, arg;
        ss >> command;
        getline(ss, arg);
        
        // Trim leading space from arg
        if (!arg.empty() && arg[0] == ' ') arg = arg.substr(1);
        
        if (command == "go" || command == "n" || command == "north" || 
            command == "s" || command == "south" || command == "e" || 
            command == "east" || command == "w" || command == "west") {
            
            string direction = command;
            if (command == "go") direction = arg;
            else if (command == "n") direction = "north";
            else if (command == "s") direction = "south";
            else if (command == "e") direction = "east";
            else if (command == "w") direction = "west";
            
            move(direction);
        }
        else if (command == "a" || command == "attack") {
            attack();
        }
        else if (command == "k" || command == "key") {
            key();
        }
        else if (command == "t" || command == "throw") {
            throwItem(arg);
        }
        else if (command == "take" || command == "get") {
            takeItem(arg);
        }
        else if (command == "inventory" || command == "i") {
            showInventory();
        }
        else if (command == "look" || command == "l") {
            if (arg == "room" || arg.empty()) {
                printRoom();
            } 
            else if (arg == "enemy") {
                string enemy = getScreenEnemy(playerY, playerX);
                if (enemy != "none") {
                    cout << "\n" << enemyDatabase[enemy].description << "\n";
                } else {
                    cout << "\nThere's no enemy here\n";
                }
                
            }
        }
        else if (command == "help" || command == "?") {
            showHelp();
        }
        else if (command == "quit" || command == "exit") {
            cout << "\nThanks for playing Castle Adventure II!\n";
            exit(0);
        }
        else {
            cout << "\nI don't understand that command. Try 'help'.\n";
        }
    }

    void move(string direction) {
        Room& currentRoom = rooms[playerY][playerX];
        string enemyKey = getScreenEnemy(playerY, playerX);
        bool enemyBlocking = false;
        
        if (enemyKey != "none") {
            Enemy& currentEnemy = enemyDatabase[enemyKey];
            enemyBlocking = currentEnemy.aggro;
        }
        
        int newX = playerX;
        int newY = playerY;
        bool canMove = false;
        
        currentRoom.visited = true;
        if(currentRoom.visitEvent == "altDisp=true") {
            currentRoom.altDisp = true;
        }

        if (direction == "north" && currentRoom.exitNorth && !enemyBlocking) {
            newY--;
            canMove = true;
        }
        else if (direction == "south" && currentRoom.exitSouth && !enemyBlocking) {
            newY++;
            canMove = true;
        }
        else if (direction == "east" && currentRoom.exitEast && !enemyBlocking) {
            newX++;
            canMove = true;
        }
        else if (direction == "west" && currentRoom.exitWest && !enemyBlocking) {
            newX--;
            canMove = true;
        }
        
        // Bounds checking
        bool oob;
        if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_HEIGHT) {
            canMove = false;
        }

        // Check if room exists at destination
        if (canMove && rooms[newY][newX].exists and !oob) {
            playerX = newX;
            playerY = newY;
            printRoom();
        }
        else {
            cout << "\nYou can't go that way.\n";
            if (enemyBlocking && enemyKey != "none") {
                cout << "(the " << enemyDatabase[enemyKey].name << " is blocking you)";
            }
        }
    }
    
    string getEquippedWeapon() {
        for (const auto& itemName : inventory) {
            if (itemDatabase[itemName].isWeapon) {
                return itemName;  // Return first weapon found
            }
        }
        return "";  // No weapon found
    }

    string getKey() {
        for (const auto& itemName : inventory) {
            if (itemDatabase[itemName].isKey) {
                return itemName;  // Return first key found
            }
        }
        return "";  // No weapon found
    }


    string getScreenEnemy(int yPos, int xPos) {
        // Check all enemies to see if any are at this position
        for (const auto& enemyName : bestiary) {
            // Check if enemy exists in database first (safe check)
            if (enemyDatabase.count(enemyName) == 0) continue;
            
            const Enemy& enemy = enemyDatabase[enemyName];
            
            // Check if this enemy is at the player's position AND not defeated
            if (enemy.enemyX == xPos && enemy.enemyY == yPos && !enemy.defeated) {
                return enemyName;
            }
        }
        return "none";  // No enemy at this location
    }


    void attack() {
        string weapon = getEquippedWeapon();
        string enemy = getScreenEnemy(playerY, playerX);


        if (weapon.empty()) {
            cout << "\nYou need a weapon to attack!\n";
            return;
        }

        if (enemy == "none") { 
            cout << "\nNo enemy to attack (moron)\n";
            return; 
        }

        // Create references - no copying, just aliases
        Enemy& targetEnemy = enemyDatabase[enemy];
        Item& weaponItem = itemDatabase[weapon];
        
        // player turn
        int damage = weaponItem.attackDmg;
        targetEnemy.hp -= damage;
        
        cout << "\nYou swing your " << weaponItem.name 
            << " for " << damage << " damage!\n";
        
        if (targetEnemy.hp <= 0) {
            targetEnemy.hp = 0;
            targetEnemy.defeated = true;  // Don't forget to mark as defeated!
            cout << "\n" << targetEnemy.defeatedMessage << "\n";
            triggerEvent(targetEnemy.event);
        }

        // enemy turn
        if (!targetEnemy.defeated) {
            damage = targetEnemy.atk;
            player.hp -= damage;
            cout << "\n" << targetEnemy.name << " attacks " << player.name << " for " << damage << " damage!\n";

            if (player.hp <= 0) {
                player.hp = 0;
                gameover = true;
            }
        }
        
        if (!gameover) {
            printRoom();
        }
 
    }

    void throwItem(string itemName) {
        // Check if player has the item
        auto it = find(inventory.begin(), inventory.end(), itemName);
        if (it == inventory.end()) {
            cout << "\nYou don't have a " << itemName << " to throw!\n";
            return;
        }
        
        // Check if there's an enemy to throw at
        string enemy = getScreenEnemy(playerY, playerX);
        if (enemy == "none") {
            cout << "\nThere's nothing to throw at!\n";
            return;
        }
        
        // Get references
        Enemy& targetEnemy = enemyDatabase[enemy];
        Item& thrownItem = itemDatabase[itemName];
        
        // Calculate damage
        int damage = thrownItem.throwDmg;
        targetEnemy.hp -= damage;
        
        // Remove item from inventory (it's been thrown!)
        inventory.erase(it);
        
        // Add item to current room (it's on the ground now)
        rooms[playerY][playerX].items.push_back(itemName);
        
        cout << "\nYou throw the " << thrownItem.name 
            << " at the " << targetEnemy.name 
            << " for " << damage << " damage!\n";
        
        if (targetEnemy.hp <= 0) {
            targetEnemy.hp = 0;
            targetEnemy.defeated = true;
            cout << "\n" << targetEnemy.defeatedMessage << "\n";
            triggerEvent(targetEnemy.event);
        }
        
        printRoom();
    }

    void key() {
        Room& currentRoom = rooms[playerY][playerX];
        string key = getKey();
        if(currentRoom.key == key) {
            cout << "\nUsed " << itemDatabase[key].name << ".\n";
            triggerEvent(currentRoom.keyEvent);
        }

        printRoom();
    }

    void triggerEvent(string event) {
        Room& currentRoom = rooms[playerY][playerX];
        if (event == "exitNorth=true") {
            currentRoom.exitNorth = true;
        }
        if (event == "exitEast=true") {
            currentRoom.exitEast= true;
        }
        if (event == "exitSouth=true") {
            currentRoom.exitSouth = true;
        }
        if (event == "exitWest=true") {
            currentRoom.exitWest = true;
        }
        if (event == "nextLevel") {
            nextLevel();
        }
        currentRoom.altDisp = true;
    }

    void nextLevel() {
        cout << "\nWelcome to the next level...\n";
        enterPause();
        currentLevel++;
        initializeLevel(currentLevel);
        initializeMap();
    }

    void takeItem(string itemName) {
        Room& room = rooms[playerY][playerX];
        
        auto it = find(room.items.begin(), room.items.end(), itemName);
        if (it != room.items.end()) {
            inventory.push_back(*it);
            room.items.erase(it);
            cout << "\nYou take the " << itemDatabase[itemName].name << ".\n";
            room.altDisp = true;
        } else {
            cout << "\nThere's no " << itemName << " here.\n";
        }
        printRoom();
    }
    
    
    void showInventory() {
        cout << "\n--- Inventory ---\n";
        if (inventory.empty()) {
            cout << "You're not carrying anything.\n";
        } else {
            for (const auto& item : inventory) {
                cout << "- " << itemDatabase[item].name << "\n";
            }
        }
    }
    
    void showHelp() {
        cout << "\n--- Commands ---\n";
        cout << "go <direction> or n/s/e/w - Move in a direction\n";
        cout << "take <item> - Pick up an item\n";
        cout << "inventory or i - Show what you're carrying\n";
        cout << "look or l - Look around the room\n";
        cout << "help or ? - Show this help\n";
        cout << "quit - Exit the game\n";
    }
    
    void showIntro() {
        cout << R"(
           C A S T L E   A D V E N T U R E                                                                   
                
                  IIIIIIIIIIIII  
                    II     II
                    II     II
                    II     II
                    II     II
                  IIIIIIIIIIIII
              
                GILDERHINF'S RETURN

        30 Years ago, in the land of Rithman, you defeated the 
        Evil Wizard Gilderhinf. A lot has changed since then, 
        but one thing has not. Gilderhinf made sure of that when 
        he set the Resurrection Timer right before he blew
        up in a slew of ****'s. And wouldn't you know it, he
        set it for 30 years. 

        Fuck.
        )" << "\n";

        enterPause();
        cout << R"(        Also you saved the princess on the
        way home from your first adventure, just to tie up that
        loose end. Take note Dave and D.B.)" << "\n";
    }
    
    void run() {
        showIntro();
        printRoom();
        
        string input;
        while (!gameover) {
            cout << "\n> ";
            getline(cin, input);
            if (!input.empty()) {
                processCommand(input);
            }
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}