#ifdef NATIVE_SIM

#include "native_mocks.h"

// Forward declarations from HeishaMon.ino
extern void setup();
extern void loop();

int main() {
    std::cout << "HeishaMon Native Simulation Starting..." << std::endl;
    std::cout << "Initializing mock hardware environment" << std::endl;
    
    // Create the tmp directory for simulated filesystem
    system("mkdir -p /tmp/heishamon_sim");
    
    // Call Arduino-style setup
    std::cout << "Calling setup()..." << std::endl;
    setup();
    
    std::cout << "Setup completed. Starting main loop..." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    
    // Run the main loop for a limited time or until interrupted
    int loop_count = 0;
    const int max_loops = 100; // Limit loops for testing
    
    while (loop_count < max_loops) {
        loop();
        loop_count++;
        
        // Add some delay to prevent spinning too fast
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (loop_count % 10 == 0) {
            std::cout << "Loop " << loop_count << " completed..." << std::endl;
        }
    }
    
    std::cout << "HeishaMon Native Simulation completed after " << loop_count << " loops." << std::endl;
    return 0;
}

#endif // NATIVE_SIM