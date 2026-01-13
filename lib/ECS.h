#ifndef DATAORIENTEDDESIGNINGAMEDEV_ECS_H
#define DATAORIENTEDDESIGNINGAMEDEV_ECS_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <algorithm>
#include <cstdint>

using EntityID = uint32_t;

struct Component {
    virtual ~Component() = default;
};

class ComponentArray {
public:
    virtual ~ComponentArray() = default;
    virtual void onEntityDestroyed(EntityID entity) = 0;
};

template<typename T>
class ComponentArrayTyped : public ComponentArray {
private:
    std::vector<T> components;
    std::unordered_map<EntityID, size_t> entityToIndex;
    std::unordered_map<size_t, EntityID> indexToEntity;

public:
    void addComponent(const EntityID entity, const T& component) {
        const size_t index = components.size();
        components.push_back(component);
        entityToIndex[entity] = index;
        indexToEntity[index] = entity;
    }

    void removeComponent(const EntityID entity) {
        if (entityToIndex.find(entity) == entityToIndex.end())
            return;

        size_t removeIndex = entityToIndex[entity];
        size_t lastIndex = components.size() - 1;

        if (removeIndex != lastIndex) {
            components[removeIndex] = components[lastIndex];
            const EntityID lastEntity = indexToEntity[lastIndex];
            entityToIndex[lastEntity] = removeIndex;
            indexToEntity[removeIndex] = lastEntity;
        }

        components.pop_back();
        entityToIndex.erase(entity);
        indexToEntity.erase(lastIndex);
    }

    T* getComponent(const EntityID entity) {
        if (entityToIndex.find(entity) == entityToIndex.end())
            return nullptr;
        return &components[entityToIndex[entity]];
    }

    std::vector<EntityID> getEntities() const {
        std::vector<EntityID> entities;
        for (const auto& [entity, _] : entityToIndex)
            entities.push_back(entity);
        return entities;
    }

    void onEntityDestroyed(const EntityID entity) override {
        removeComponent(entity);
    }
};

class ECSWorld {
private:
    EntityID nextEntityID = 1;
    std::unordered_map<std::type_index, std::shared_ptr<ComponentArray>> componentArrays;
    std::unordered_map<EntityID, std::vector<std::type_index>> entityComponents;

public:
    EntityID createEntity() {
        return nextEntityID++;
    }

    void destroyEntity(const EntityID entity) {
        if (entityComponents.find(entity) == entityComponents.end())
            return;

        for (const auto& typeIdx : entityComponents[entity]) {
            componentArrays[typeIdx]->onEntityDestroyed(entity);
        }
        entityComponents.erase(entity);
    }

    template<typename T>
    void addComponent(EntityID entity, const T& component) {
        const std::type_index typeIdx(typeid(T));

        if (componentArrays.find(typeIdx) == componentArrays.end()) {
            componentArrays[typeIdx] = std::make_shared<ComponentArrayTyped<T>>();
        }

        auto* array = static_cast<ComponentArrayTyped<T>*>(componentArrays[typeIdx].get());
        array->addComponent(entity, component);

        entityComponents[entity].push_back(typeIdx);
    }

    template<typename T>
    void removeComponent(EntityID entity) {
        const std::type_index typeIdx(typeid(T));

        if (componentArrays.find(typeIdx) == componentArrays.end())
            return;

        auto* array = static_cast<ComponentArrayTyped<T>*>(componentArrays[typeIdx].get());
        array->removeComponent(entity);

        auto& components = entityComponents[entity];
        components.erase(std::remove(components.begin(), components.end(), typeIdx), components.end());
    }

    template<typename T>
    T* getComponent(EntityID entity) {
        const std::type_index typeIdx(typeid(T));

        if (componentArrays.find(typeIdx) == componentArrays.end())
            return nullptr;

        auto* array = static_cast<ComponentArrayTyped<T>*>(componentArrays[typeIdx].get());
        return array->getComponent(entity);
    }

    template<typename T>
    std::vector<EntityID> getEntitiesWith() {
        const std::type_index typeIdx(typeid(T));

        if (componentArrays.find(typeIdx) == componentArrays.end())
            return {};

        auto* array = static_cast<ComponentArrayTyped<T>*>(componentArrays[typeIdx].get());
        return array->getEntities();
    }
};

#endif