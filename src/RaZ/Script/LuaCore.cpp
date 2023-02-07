#include "RaZ/Application.hpp"
#include "RaZ/Component.hpp"
#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Data/BvhSystem.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerCoreTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Application> application = state.new_usertype<Application>("Application",
                                                                             sol::constructors<Application(),
                                                                                               Application(std::size_t)>());
    application["getWorlds"]    = PickNonConstOverload<>(&Application::getWorlds);
    application["getDeltaTime"] = &Application::getDeltaTime;
    application["addWorld"]     = &Application::addWorld<>;
    application["run"]          = sol::overload([] (Application& app) { app.run(); },
                                                [] (Application& app, const sol::protected_function& func) {
      app.run([&func] (float deltaTime) {
        const auto result = func(deltaTime);

        if (!result.valid()) {
            sol::error err = result;
            throw std::runtime_error(std::move(err));
        }
      });
    });
    application["runOnce"]      = &Application::runOnce;
    application["quit"]         = &Application::quit;
  }

  {
    state.new_usertype<Component>("Component", sol::no_constructor);
  }

  {
    sol::usertype<Entity> entity = state.new_usertype<Entity>("Entity",
                                                              sol::constructors<Entity(std::size_t),
                                                                                Entity(std::size_t, bool)>());
    entity["getId"]                = &Entity::getId;
    entity["isEnabled"]            = &Entity::isEnabled;
    entity["getEnabledComponents"] = &Entity::getEnabledComponents;
    entity["enable"]               = sol::overload([] (Entity& e) { e.enable(); },
                                                   PickOverload<bool>(&Entity::enable));
    entity["disable"]              = &Entity::disable;
    // TODO: bind components
  }

  {
    sol::usertype<System> system = state.new_usertype<System>("System", sol::no_constructor);
    system["getAcceptedComponents"] = &System::getAcceptedComponents;
    system["containsEntity"]        = &System::containsEntity;
    system["update"]                = &System::update;
    system["step"]                  = &System::step;
    system["destroy"]               = &System::destroy;
  }

  {
    sol::usertype<World> world = state.new_usertype<World>("World",
                                                           sol::constructors<World(),
                                                                             World(std::size_t)>());
    world["addAudioSystem"]   = sol::overload(&World::addSystem<AudioSystem>,
                                              &World::addSystem<AudioSystem, const char*>);
    world["addBvhSystem"]     = &World::addSystem<BvhSystem>;
    world["addPhysicsSystem"] = &World::addSystem<PhysicsSystem>;
    world["addRenderSystem"]  = sol::overload(&World::addSystem<RenderSystem>,
                                              &World::addSystem<RenderSystem, unsigned int, unsigned int>
#if !defined(RAZ_NO_WINDOW)
                                              ,
                                              &World::addSystem<RenderSystem, unsigned int, unsigned int, const std::string&>,
                                              &World::addSystem<RenderSystem, unsigned int, unsigned int, const std::string&, WindowSetting>,
                                              &World::addSystem<RenderSystem, unsigned int, unsigned int, const std::string&, WindowSetting, uint8_t>
#endif
                                              );
    world["addEntity"]        = sol::overload([] (World& w) { return &w.addEntity(); },
                                              PickOverload<bool>(&World::addEntity));
    world["removeEntity"]     = &World::removeEntity;
    world["update"]           = &World::update;
    world["refresh"]          = &World::refresh;
    world["destroy"]          = &World::destroy;
  }
}

} // namespace Raz
