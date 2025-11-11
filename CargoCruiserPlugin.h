#pragma once

#include "FreightPlugin.h"
#include <iostream>

// CargoCruiser freight type plugin
class CargoCruiserPlugin : public FreightPlugin {
public:
    std::string getPluginName() const override {
        return "CargoCruiser Plugin";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
    std::string getFreightTypeName() const override {
        return "CargoCruiser";
    }
    
    int getBaseCapacity() const override {
        return 50; // Base capacity: 50 units
    }
    
    double getCostPerUnit() const override {
        return 2.50; // $2.50 per unit
    }
    
    double getSpeedFactor() const override {
        return 1.0; // Standard speed
    }
    
    bool validateCargo(const Cargo& cargo) const override {
        // Handles cargo 50-500 weight
        int capacity = cargo.getCapacity();
        return capacity >= 50 && capacity <= 500;
    }
    
    bool canTransport(const std::string& destination) const override {
        // CargoCruiser can go anywhere
        return true;
    }
    
    void onLoad(const freight& f, const Cargo& c) const override {
        std::cout << "📦 CargoCruiser F" << f.getIndex() 
                  << " loading Cargo C" << c.getIndex() << " (Balanced transport)\n";
    }
    
    void onUnload(const freight& f, const Cargo& c) const override {
        std::cout << "✓ CargoCruiser F" << f.getIndex() 
                  << " delivered Cargo C" << c.getIndex() << "\n";
    }
    
    std::string getCustomInfo() const override {
        return "Balanced capacity and speed";
    }
};

// Auto-register the plugin
REGISTER_FREIGHT_PLUGIN(CargoCruiserPlugin, "CargoCruiser")
