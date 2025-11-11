#pragma once

#include "FreightPlugin.h"
#include <iostream>

// MiniMover freight type plugin
class MiniMoverPlugin : public FreightPlugin {
public:
    std::string getPluginName() const override {
        return "MiniMover Plugin";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
    std::string getFreightTypeName() const override {
        return "MiniMover";
    }
    
    int getBaseCapacity() const override {
        return 20; // Base capacity: 20 units
    }
    
    double getCostPerUnit() const override {
        return 1.50; // $1.50 per unit
    }
    
    double getSpeedFactor() const override {
        return 1.2; // 20% faster than standard
    }
    
    bool validateCargo(const Cargo& cargo) const override {
        // Only handles cargo <= 100 weight
        return cargo.getCapacity() <= 100;
    }
    
    bool canTransport(const std::string& destination) const override {
        // MiniMover can go anywhere
        return true;
    }
    
    void onLoad(const freight& f, const Cargo& c) const override {
        std::cout << "📦 MiniMover F" << f.getIndex() 
                  << " loading Cargo C" << c.getIndex() << " (Fast delivery)\n";
    }
    
    void onUnload(const freight& f, const Cargo& c) const override {
        std::cout << "✓ MiniMover F" << f.getIndex() 
                  << " delivered Cargo C" << c.getIndex() << "\n";
    }
    
    std::string getCustomInfo() const override {
        return "Fast delivery for small packages";
    }
};

// Auto-register the plugin
REGISTER_FREIGHT_PLUGIN(MiniMoverPlugin, "MiniMover")
