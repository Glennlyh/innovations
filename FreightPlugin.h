#pragma once

#include "freight.h"
#include "Cargo.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

// Abstract base class for freight plugins
class FreightPlugin {
public:
    virtual ~FreightPlugin() = default;
    
    // Plugin metadata
    virtual std::string getPluginName() const = 0;
    virtual std::string getPluginVersion() const = 0;
    virtual std::string getFreightTypeName() const = 0;
    
    // Freight properties
    virtual int getBaseCapacity() const = 0;
    virtual double getCostPerUnit() const = 0;
    virtual double getSpeedFactor() const = 0;
    
    // Validation methods
    virtual bool validateCargo(const Cargo& cargo) const = 0;
    virtual bool canTransport(const std::string& destination) const = 0;
    
    // Lifecycle hooks
    virtual void onLoad(const freight& f, const Cargo& c) const = 0;
    virtual void onUnload(const freight& f, const Cargo& c) const = 0;
    
    // Additional information
    virtual std::string getCustomInfo() const = 0;
};

// Factory for creating freight plugins
class FreightPluginFactory {
public:
    using PluginCreator = std::function<std::unique_ptr<FreightPlugin>()>;
    
    // Singleton instance
    static FreightPluginFactory& getInstance();
    
    // Register a plugin creator
    void registerPlugin(const std::string& typeName, PluginCreator creator);
    
    // Create a plugin instance
    std::unique_ptr<FreightPlugin> createPlugin(const std::string& typeName) const;
    
    // Get list of registered plugin types
    std::vector<std::string> getRegisteredTypes() const;
    
    // Check if a plugin is registered
    bool isRegistered(const std::string& typeName) const;
    
private:
    FreightPluginFactory() = default;
    ~FreightPluginFactory() = default;
    
    // Prevent copying
    FreightPluginFactory(const FreightPluginFactory&) = delete;
    FreightPluginFactory& operator=(const FreightPluginFactory&) = delete;
    
    std::unordered_map<std::string, PluginCreator> creators_;
};

// Helper class for automatic plugin registration
template<typename T>
class PluginRegistrar {
public:
    PluginRegistrar(const std::string& typeName) {
        FreightPluginFactory::getInstance().registerPlugin(
            typeName,
            []() -> std::unique_ptr<FreightPlugin> {
                return std::unique_ptr<FreightPlugin>(new T());
            }
        );
    }
};

// Macro for easy plugin registration
#define REGISTER_FREIGHT_PLUGIN(ClassName, TypeName) \
    static PluginRegistrar<ClassName> ClassName##_registrar(TypeName);
