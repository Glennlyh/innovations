#include "FreightPlugin.h"
#include <iostream>

using namespace std;

FreightPluginFactory& FreightPluginFactory::getInstance() {
    static FreightPluginFactory instance;
    return instance;
}

void FreightPluginFactory::registerPlugin(const string& typeName, PluginCreator creator) {
    if (creators_.find(typeName) != creators_.end()) {
        cout << "⚠️  Plugin '" << typeName << "' is already registered. Overwriting...\n";
    }
    creators_[typeName] = creator;
    cout << "✓ Plugin '" << typeName << "' registered successfully.\n";
}

unique_ptr<FreightPlugin> FreightPluginFactory::createPlugin(const string& typeName) const {
    auto it = creators_.find(typeName);
    if (it != creators_.end()) {
        return it->second();
    }
    cerr << "❌ Plugin '" << typeName << "' not found!\n";
    return nullptr;
}

vector<string> FreightPluginFactory::getRegisteredTypes() const {
    vector<string> types;
    for (const auto& pair : creators_) {
        types.push_back(pair.first);
    }
    return types;
}

bool FreightPluginFactory::isRegistered(const string& typeName) const {
    return creators_.find(typeName) != creators_.end();
}
