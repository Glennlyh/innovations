#pragma once

#include "FreightPlugin.h"
#include <iostream>

// MegaCarrier freight type plugin
class MegaCarrierPlugin : public FreightPlugin {
public:
    std::string getPluginName() const override {
        return "MegaCarrier Plugin";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
    std::string getFreightTypeName() const override {
        return "MegaCarrier";
    }
    
    int getBaseCapacity() const override {
        return 100; // Base capacity: 100 units
    }
    
    double getCostPerUnit() const override {
        return 4.00; // $4.00 per unit
    }
    
    double getSpeedFactor() const override {
        return 0.8; // 20% slower but larger capacity
    }
    
    bool validateCargo(const Cargo& cargo) const override {
        // Handles cargo 200-2000 weight
        int capacity = cargo.getCapacity();
        return capacity >= 200 && capacity <= 2000;
    }
    
    bool canTransport(const std::string& destination) const override {
        // MegaCarrier can go anywhere
        return true;
    }
    
    void onLoad(const freight& f, const Cargo& c) const override {
        std::cout << "📦 MegaCarrier F" << f.getIndex() 
                  << " loading Cargo C" << c.getIndex() << " (Bulk shipment)\n";
    }
    
    void onUnload(const freight& f, const Cargo& c) const override {
        std::cout << "✓ MegaCarrier F" << f.getIndex() 
                  << " delivered Cargo C" << c.getIndex() << "\n";
    }
    
    std::string getCustomInfo() const override {
        return "Maximum capacity for bulk shipments";
    }
};

// Auto-register the plugin
REGISTER_FREIGHT_PLUGIN(MegaCarrierPlugin, "MegaCarrier")
