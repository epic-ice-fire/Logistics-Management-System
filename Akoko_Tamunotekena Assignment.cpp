#include <iostream>
#include <string>
#include <list>         // Implements Doubly Linked List for dynamic records [3, 4]
#include <stack>        // Implements LIFO principle for Undo/Redo [5, 6]
#include <queue>        // Provides Priority Queue for prioritized loading [7, 8]
#include <vector>       // Used as a dynamic Array for audit/reporting [1, 9]
#include <limits>       // For input cleaning

// Define the Parcel Structure (Requirement 1)
struct Parcel {
    int id;
    std::string sender;
    std::string recipient;
    std::string address;
    double weight;
    int priority; // E.g., 1 (High) to 5 (Low)

    // Operator overload required for Priority Queue [7]: 
    // Ensures smaller priority number (higher priority) is served first [10].
    bool operator<(const Parcel& other) const {
        return priority > other.priority; 
    }
};

struct Action {
    std::string type; 
    Parcel data;      // Stores the state BEFORE the action was taken (for reversal)
};

class JumiaLogisticsManager {
private:
    // Linked List for dynamic storage, updates, and removal [11, 12]
    std::list<Parcel> active_parcels;           
    
    // Priority Queue for organized loading and urgent delivery handling [7, 12]
    std::priority_queue<Parcel> loading_queue;  
    
    // Stack for undo/redo based on LIFO principle [5, 12]
    std::stack<Action> undo_stack;              
    
    // Dynamic Array (Vector) for delivered parcels and audit trail [9, 12]
    std::vector<Parcel> delivered_parcels;      

    // Helper function for the Stack (Push operation) [5, 6, 13]
    void record_action(const std::string& type, const Parcel& p) {
        Action act = {type, p};
        undo_stack.push(act);
    }

public:
    // MOVED TO PUBLIC to allow interaction/error handling from main()
    void clear_input() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

   
    void register_parcel_interactive() {
        Parcel p;
        std::cout << "\n--- Register New Parcel ---" << std::endl;
        std::cout << "Enter Parcel ID: ";
        if (!(std::cin >> p.id)) { clear_input(); std::cout << "Invalid ID." << std::endl; return; }
        
        std::cout << "Enter Sender Name: "; 
        std::cin >> p.sender;
        
        std::cout << "Enter Recipient Name: ";
        std::cin >> p.recipient;
        
        std::cout << "Enter Address (no spaces): ";
        std::cin >> p.address;
        
        std::cout << "Enter Weight (kg): ";
        if (!(std::cin >> p.weight)) { clear_input(); std::cout << "Invalid weight." << std::endl; return; }
        
        std::cout << "Enter Delivery Priority (1=High, 5=Low): ";
        if (!(std::cin >> p.priority) || p.priority < 1 || p.priority > 5) { 
            clear_input(); 
            std::cout << "Invalid priority. Must be between 1 and 5." << std::endl; 
            return; 
        }

        active_parcels.push_back(p); // Insertion at the end of the list [15]
        record_action("ADD", p);
        std::cout << "\nSUCCESS: Parcel " << p.id << " registered and recorded for undo." << std::endl;
    }

    // 2. Update Parcel (Linked List Traversal and Update) [12]
    void update_parcel_interactive() {
        int id;
        double new_weight;
        std::cout << "\nEnter Parcel ID to Update: ";
        if (!(std::cin >> id)) { clear_input(); std::cout << "Invalid input." << std::endl; return; }

        for (auto it = active_parcels.begin(); it != active_parcels.end(); ++it) {
            if (it->id == id) {
                Parcel old_data = *it; // Store state for undo (UPDATE operation records previous state)
                
                std::cout << "Enter New Weight for P" << id << " (Current: " << it->weight << "): ";
                if (!(std::cin >> new_weight)) { clear_input(); std::cout << "Invalid input." << std::endl; return; }
                
                it->weight = new_weight; // Update element [16]
                record_action("UPDATE", old_data);
                std::cout << "\nSUCCESS: Parcel " << id << " updated." << std::endl;
                return;
            }
        }
        std::cout << "\nError: Parcel ID " << id << " not found in active records." << std::endl;
    }

    // 3. Load Parcel (Priority Queue Enqueue)
    void prepare_for_loading_interactive() {
        int id;
        std::cout << "\nEnter Parcel ID to Load onto truck: ";
        if (!(std::cin >> id)) { clear_input(); std::cout << "Invalid input." << std::endl; return; }

        for (const auto& p : active_parcels) {
            if (p.id == id) {
                loading_queue.push(p); // Enqueue based on priority
                std::cout << "\nSUCCESS: Parcel " << id << " loaded (Priority: " << p.priority << "). Will be dispatched based on urgency." << std::endl;
                return;
            }
        }
        std::cout << "\nError: Parcel ID " << id << " not found." << std::endl;
    }

    // 4. Dispatch Next Parcel (Priority Queue Dequeue)
    void dispatch_next_parcel() {
        if (loading_queue.empty()) {
            std::cout << "\nERROR: Loading queue is empty. (Underflow) [18]." << std::endl;
            return;
        }
        // Dequeue: Access and remove the highest priority item from the front [7, 17]
        Parcel next_dispatch = loading_queue.top();
        loading_queue.pop(); 
        std::cout << "\nDISPATCH SUCCESS: Parcel ID " << next_dispatch.id << " (Priority " << next_dispatch.priority << ") dispatched immediately." << std::endl;
    }
    
    // 5. Complete Delivery (Linked List Deletion & Array/Vector Insertion) [12, 19]
    void complete_delivery_interactive() {
        int id;
        std::cout << "\nEnter Parcel ID to mark as delivered: ";
        if (!(std::cin >> id)) { clear_input(); std::cout << "Invalid input." << std::endl; return; }

        for (auto it = active_parcels.begin(); it != active_parcels.end(); ++it) {
            if (it->id == id) {
                Parcel delivered_p = *it;

                delivered_parcels.push_back(delivered_p); // Audit Array insertion (Requirement 5)
                active_parcels.erase(it); // Linked List deletion [19]

                record_action("DELETE", delivered_p); // Record deleted item for potential reversal
                std::cout << "\nSUCCESS: Parcel " << id << " marked delivered and removed from active list." << std::endl;
                return;
            }
        }
        std::cout << "\nError: Parcel ID " << id << " not found in active list." << std::endl;
    }

    void undo_last_action() {
        if (undo_stack.empty()) {
            std::cout << "\nNO UNDO: Stack is empty (Underflow) [13]. No recent actions recorded." << std::endl;
            return;
        }

        // Pop the last action (LIFO) [5, 6]
        Action last_action = undo_stack.top();
        undo_stack.pop();

        std::cout << "\n--- Undoing Action: " << last_action.type << " on Parcel ID " << last_action.data.id << " ---" << std::endl;

        // Reversal Logic:
        if (last_action.type == "ADD") {
            // Reverse an ADD: Delete the item added (must search the list) [19]
            for (auto it = active_parcels.begin(); it != active_parcels.end(); ++it) {
                if (it->id == last_action.data.id) {
                    active_parcels.erase(it);
                    std::cout << "UNDO SUCCESS: Registered Parcel " << last_action.data.id << " removed from active list." << std::endl;
                    return;
                }
            }
        } else if (last_action.type == "DELETE") {
            // Reverse a DELETE: Re-insert the parcel into the active list [20]
            active_parcels.push_back(last_action.data);
            // Note: A full undo requires removing the item from delivered_parcels as well.
            std::cout << "UNDO SUCCESS: Parcel " << last_action.data.id << " restored to active list." << std::endl;
        } else if (last_action.type == "UPDATE") {
            // Reverse an UPDATE: Restore the old data saved in 'last_action.data' [16]
            for (auto& p : active_parcels) {
                if (p.id == last_action.data.id) {
                    p.weight = last_action.data.weight; // Restore old weight
                    std::cout << "UNDO SUCCESS: Parcel " << last_action.data.id << " weight restored to " << p.weight << "." << std::endl;
                    return;
                }
            }
        }
    }

    // 7. Generate Summary Reports (Array/Vector and Linked List Traversal) [12]
    void generate_summary_reports() const {
        double total_weight = 0.0;
        int total_registered = active_parcels.size() + delivered_parcels.size();
        
        // Use an array (vector) to count pending parcels by priority
        std::vector<int> pending_by_priority(6, 0); 
        
        // Traversal of active parcels (Linked List)
        for (const auto& p : active_parcels) {
            total_weight += p.weight;
            if (p.priority >= 1 && p.priority <= 5) {
                pending_by_priority[p.priority]++;
            }
        }
        // Traversal of delivered parcels (Array/Vector)
        for (const auto& p : delivered_parcels) {
            total_weight += p.weight;
        }

        std::cout << "\n--- JUMIA LOGISTICS SUMMARY REPORT ---" << std::endl;
        std::cout << "Total Parcels Registered: " << total_registered << std::endl;
        std::cout << "Total Parcels Delivered: " << delivered_parcels.size() << std::endl;
        
        // Average parcel weight calculation
        if (total_registered > 0) {
            std::cout << "Average Parcel Weight: " << total_weight / total_registered << " kg" << std::endl;
        }
        
        // Parcels pending delivery by priority level
        std::cout << "\nParcels Pending by Priority Level:" << std::endl;
        for (int i = 1; i <= 5; ++i) {
            std::cout << "  Priority " << i << ": " << pending_by_priority[i] << std::endl;
        }
        
        // Delivery History and Route Summary
        std::cout << "\nDelivery History (Audit Trail - Delivered Parcels):" << std::endl;
        if (delivered_parcels.empty()) {
            std::cout << "  No deliveries completed yet." << std::endl;
        } else {
            for (const auto& p : delivered_parcels) {
                std::cout << "  [DELIVERED] P" << p.id << " to " << p.recipient << " (P" << p.priority << ")" << std::endl;
            }
        }
        std::cout << "--------------------------------------" << std::endl;
    }
    
    // 8. Display Menu
    void display_menu() const {
        std::cout << "\n=======================================" << std::endl;
        std::cout << "JUMIA LOGISTICS MANAGEMENT SYSTEM" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << "1. Register New Parcel (Linked List Insert)" << std::endl;
        std::cout << "2. Update Parcel Weight (Linked List Search/Update)" << std::endl;
        std::cout << "3. Prepare for Loading (Priority Queue Enqueue)" << std::endl;
        std::cout << "4. Dispatch Next Parcel (Priority Queue Dequeue)" << std::endl;
        std::cout << "5. Complete Delivery (Linked List Delete & Array Audit)" << std::endl;
        std::cout << "6. Undo Last Action (Stack Pop/LIFO)" << std::endl;
        std::cout << "7. Generate Summary Reports" << std::endl;
        std::cout << "0. Exit Program" << std::endl;
        std::cout << "Enter choice: ";
    }
};

int main() {
    JumiaLogisticsManager manager;
    int choice;

    do {
        manager.display_menu();
        if (!(std::cin >> choice)) {
            // Error handling calls the now PUBLIC clear_input()
            manager.clear_input(); 
            choice = -1; 
            std::cout << "Invalid input type. Please enter a number." << std::endl;
        }

        switch (choice) {
            case 1: manager.register_parcel_interactive(); break;
            case 2: manager.update_parcel_interactive(); break;
            case 3: manager.prepare_for_loading_interactive(); break;
            case 4: manager.dispatch_next_parcel(); break;
            case 5: manager.complete_delivery_interactive(); break;
            case 6: manager.undo_last_action(); break;
            case 7: manager.generate_summary_reports(); break;
            case 0: std::cout << "Exiting Jumia Logistics System. Goodbye!" << std::endl; break;
            default: 
                if (choice != -1) { // Avoid printing error twice if input failed
                    std::cout << "Invalid choice. Please try again (0-7)." << std::endl; 
                }
                break;
        }
    } while (choice != 0);

    return 0;
}
