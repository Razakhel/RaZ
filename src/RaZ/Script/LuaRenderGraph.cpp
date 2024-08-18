#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/BloomRenderProcess.hpp"
#include "RaZ/Render/BoxBlurRenderProcess.hpp"
#include "RaZ/Render/ChromaticAberrationRenderProcess.hpp"
#include "RaZ/Render/ConvolutionRenderProcess.hpp"
#include "RaZ/Render/FilmGrainRenderProcess.hpp"
#include "RaZ/Render/GaussianBlurRenderProcess.hpp"
#include "RaZ/Render/PixelizationRenderProcess.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/ScreenSpaceReflectionsRenderProcess.hpp"
#include "RaZ/Render/SobelFilterRenderProcess.hpp"
#include "RaZ/Render/VignetteRenderProcess.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerRenderGraphTypes() {
  sol::state& state = getState();

  {
    sol::usertype<RenderGraph> renderGraph = state.new_usertype<RenderGraph>("RenderGraph",
                                                                             sol::constructors<RenderGraph()>(),
                                                                             sol::base_classes, sol::bases<Graph<RenderPass>>());
    renderGraph["getNodeCount"] = &RenderGraph::getNodeCount;
    renderGraph["getNode"]      = PickNonConstOverload<std::size_t>(&RenderGraph::getNode);
    renderGraph["addNode"]      = sol::overload(&RenderGraph::addNode<>,
                                                [] (RenderGraph& g, FragmentShader& s) { g.addNode(std::move(s)); },
                                                [] (RenderGraph& g, FragmentShader& s, std::string n) { g.addNode(std::move(s), std::move(n)); });
    renderGraph["removeNode"]   = &RenderGraph::removeNode;

    renderGraph["isValid"]                                = &RenderGraph::isValid;
    renderGraph["getGeometryPass"]                        = PickNonConstOverload<>(&RenderGraph::getGeometryPass);
    renderGraph["addBloomRenderProcess"]                  = &RenderGraph::addRenderProcess<BloomRenderProcess>;
    renderGraph["addBoxBlurRenderProcess"]                = &RenderGraph::addRenderProcess<BoxBlurRenderProcess>;
    renderGraph["addChromaticAberrationRenderProcess"]    = &RenderGraph::addRenderProcess<ChromaticAberrationRenderProcess>;
    renderGraph["addConvolutionRenderProcess"]            = sol::overload(&RenderGraph::addRenderProcess<ConvolutionRenderProcess, const Mat3f&>,
                                                                          &RenderGraph::addRenderProcess<ConvolutionRenderProcess, const Mat3f&, std::string>);
    renderGraph["addFilmGrainRenderProcess"]              = &RenderGraph::addRenderProcess<FilmGrainRenderProcess>;
    renderGraph["addGaussianBlurRenderProcess"]           = &RenderGraph::addRenderProcess<GaussianBlurRenderProcess>;
    renderGraph["addPixelizationRenderProcess"]           = &RenderGraph::addRenderProcess<PixelizationRenderProcess>;
    renderGraph["addScreenSpaceReflectionsRenderProcess"] = &RenderGraph::addRenderProcess<ScreenSpaceReflectionsRenderProcess>;
    renderGraph["addSobelFilterRenderProcess"]            = &RenderGraph::addRenderProcess<SobelFilterRenderProcess>;
    renderGraph["addVignetteRenderProcess"]               = &RenderGraph::addRenderProcess<VignetteRenderProcess>;
    renderGraph["resizeViewport"]                         = &RenderGraph::resizeViewport;
    renderGraph["updateShaders"]                          = &RenderGraph::updateShaders;
  }

  {
    sol::usertype<RenderPass> renderPass = state.new_usertype<RenderPass>("RenderPass",
                                                                          sol::constructors<RenderPass()>(),
                                                                          sol::base_classes, sol::bases<GraphNode<RenderPass>>());
    renderPass["getParentCount"] = &RenderPass::getParentCount;
    renderPass["getParent"]      = PickNonConstOverload<std::size_t>(&RenderPass::getParent);
    renderPass["getChildCount"]  = &RenderPass::getChildCount;
    renderPass["getChild"]       = PickNonConstOverload<std::size_t>(&RenderPass::getChild);
    renderPass["isRoot"]         = &RenderPass::isRoot;
    renderPass["isLeaf"]         = &RenderPass::isLeaf;
    renderPass["isIsolated"]     = &RenderPass::isIsolated;
    renderPass["addParents"]     = [] (RenderPass& p, sol::variadic_args args) { for (auto parent : args) p.addParents(parent); };
    renderPass["removeParents"]  = [] (RenderPass& p, sol::variadic_args args) { for (auto parent : args) p.removeParents(parent); };
    renderPass["addChildren"]    = [] (RenderPass& p, sol::variadic_args args) { for (auto child : args) p.addChildren(child); };
    renderPass["removeChildren"] = [] (RenderPass& p, sol::variadic_args args) { for (auto child : args) p.removeChildren(child); };

    renderPass["isEnabled"]            = &RenderPass::isEnabled;
    renderPass["getName"]              = &RenderPass::getName;
    renderPass["getProgram"]           = PickNonConstOverload<>(&RenderPass::getProgram);
    renderPass["getReadTextureCount"]  = &RenderPass::getReadTextureCount;
    renderPass["getReadTexture"]       = sol::overload([] (const RenderPass& p, std::size_t i) { return &p.getReadTexture(i); },
                                                       [] (const RenderPass& p, const std::string& n) { return &p.getReadTexture(n); });
    renderPass["hasReadTexture"]       = &RenderPass::hasReadTexture;
    renderPass["getFramebuffer"]       = [] (const RenderPass& p) { return &p.getFramebuffer(); };
    renderPass["recoverElapsedTime"]   = &RenderPass::recoverElapsedTime;
    renderPass["setName"]              = &RenderPass::setName;
    renderPass["setProgram"]           = [] (RenderPass& p, RenderShaderProgram& sp) { p.setProgram(std::move(sp)); };
    renderPass["enable"]               = sol::overload([] (RenderPass& p) { p.enable(); },
                                                       PickOverload<bool>(&RenderPass::enable));
    renderPass["disable"]              = &RenderPass::disable;
    renderPass["isValid"]              = &RenderPass::isValid;
    renderPass["addReadTexture"]       = sol::overload(
#if !defined(USE_OPENGL_ES)
                                                       [] (RenderPass& p, Texture1DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); },
#endif
                                                       [] (RenderPass& p, Texture2DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); },
                                                       [] (RenderPass& p, Texture3DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); });
    renderPass["removeReadTexture"]    = &RenderPass::removeReadTexture;
    renderPass["clearReadTextures"]    = &RenderPass::clearReadTextures;
    renderPass["setWriteDepthTexture"] = &RenderPass::setWriteDepthTexture;
    renderPass["addWriteColorTexture"] = &RenderPass::addWriteColorTexture;
    renderPass["removeWriteTexture"]   = &RenderPass::removeWriteTexture;
    renderPass["clearWriteTextures"]   = &RenderPass::clearWriteTextures;
    renderPass["resizeWriteBuffers"]   = &RenderPass::resizeWriteBuffers;
    renderPass["execute"]              = &RenderPass::execute;
  }

  // RenderProcess
  {
    {
      auto bloomRenderProcess = state.new_usertype<BloomRenderProcess>("BloomRenderProcess",
                                                                       sol::constructors<BloomRenderProcess(RenderGraph&)>(),
                                                                       sol::base_classes, sol::bases<RenderProcess>());
      bloomRenderProcess["getThresholdPass"]        = &BloomRenderProcess::getThresholdPass;
      bloomRenderProcess["getDownscalePassCount"]   = &BloomRenderProcess::getDownscalePassCount;
      bloomRenderProcess["getDownscalePass"]        = PickNonConstOverload<std::size_t>(&BloomRenderProcess::getDownscalePass);
      bloomRenderProcess["getDownscaleBufferCount"] = &BloomRenderProcess::getDownscaleBufferCount;
      bloomRenderProcess["getDownscaleBuffer"]      = [] (BloomRenderProcess& b, std::size_t i) { return &b.getDownscaleBuffer(i); };
      bloomRenderProcess["getUpscalePassCount"]     = &BloomRenderProcess::getUpscalePassCount;
      bloomRenderProcess["getUpscalePass"]          = PickNonConstOverload<std::size_t>(&BloomRenderProcess::getUpscalePass);
      bloomRenderProcess["getUpscaleBufferCount"]   = &BloomRenderProcess::getUpscaleBufferCount;
      bloomRenderProcess["getUpscaleBuffer"]        = [] (BloomRenderProcess& b, std::size_t i) { return &b.getUpscaleBuffer(i); };
      bloomRenderProcess["setInputColorBuffer"]     = &BloomRenderProcess::setInputColorBuffer;
      bloomRenderProcess["setOutputBuffer"]         = &BloomRenderProcess::setOutputBuffer;
      bloomRenderProcess["setThresholdValue"]       = &BloomRenderProcess::setThresholdValue;
    }

    {
      auto boxBlurRenderProcess = state.new_usertype<BoxBlurRenderProcess>("BoxBlurRenderProcess",
                                                                           sol::constructors<BoxBlurRenderProcess(RenderGraph&)>(),
                                                                           sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      boxBlurRenderProcess["setInputBuffer"]  = &BoxBlurRenderProcess::setInputBuffer;
      boxBlurRenderProcess["setOutputBuffer"] = &BoxBlurRenderProcess::setOutputBuffer;
      boxBlurRenderProcess["setStrength"]     = &BoxBlurRenderProcess::setStrength;
    }

    {
      auto chromAberrRenderProcess = state.new_usertype<ChromaticAberrationRenderProcess>("ChromaticAberrationRenderProcess",
                                                                                          sol::constructors<ChromaticAberrationRenderProcess(RenderGraph&)>(),
                                                                                          sol::base_classes, sol::bases<MonoPassRenderProcess,
                                                                                                                        RenderProcess>());
      chromAberrRenderProcess["setInputBuffer"]  = &ChromaticAberrationRenderProcess::setInputBuffer;
      chromAberrRenderProcess["setOutputBuffer"] = &ChromaticAberrationRenderProcess::setOutputBuffer;
      chromAberrRenderProcess["setStrength"]     = &ChromaticAberrationRenderProcess::setStrength;
      chromAberrRenderProcess["setDirection"]    = &ChromaticAberrationRenderProcess::setDirection;
      chromAberrRenderProcess["setMaskTexture"]  = &ChromaticAberrationRenderProcess::setMaskTexture;
    }

    {
      auto convolutionRenderProcess = state.new_usertype<ConvolutionRenderProcess>("ConvolutionRenderProcess",
                                                                                   sol::constructors<ConvolutionRenderProcess(RenderGraph&, const Mat3f&),
                                                                                                     ConvolutionRenderProcess(RenderGraph&, const Mat3f&,
                                                                                                                              std::string)>(),
                                                                                   sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      convolutionRenderProcess["setInputBuffer"]  = &ConvolutionRenderProcess::setInputBuffer;
      convolutionRenderProcess["setOutputBuffer"] = &ConvolutionRenderProcess::setOutputBuffer;
      convolutionRenderProcess["setKernel"]       = &ConvolutionRenderProcess::setKernel;
    }

    {
      auto gaussianBlurRenderProcess = state.new_usertype<GaussianBlurRenderProcess>("GaussianBlurRenderProcess",
                                                                                     sol::constructors<GaussianBlurRenderProcess(RenderGraph&)>(),
                                                                                     sol::base_classes, sol::bases<RenderProcess>());
      gaussianBlurRenderProcess["getHorizontalPass"] = [] (GaussianBlurRenderProcess& gb) { return &gb.getHorizontalPass(); };
      gaussianBlurRenderProcess["getVerticalPass"]   = [] (GaussianBlurRenderProcess& gb) { return &gb.getVerticalPass(); };
      gaussianBlurRenderProcess["setInputBuffer"]    = &GaussianBlurRenderProcess::setInputBuffer;
      gaussianBlurRenderProcess["setOutputBuffer"]   = &GaussianBlurRenderProcess::setOutputBuffer;
    }

    {
      state.new_usertype<MonoPassRenderProcess>("MonoPassRenderProcess", sol::no_constructor, sol::base_classes, sol::bases<RenderProcess>());
    }

    {
      sol::usertype<RenderProcess> renderProcess = state.new_usertype<RenderProcess>("RenderProcess", sol::no_constructor);
      renderProcess["isEnabled"]          = &RenderProcess::isEnabled;
      renderProcess["setState"]           = &RenderProcess::setState;
      renderProcess["enable"]             = &RenderProcess::enable;
      renderProcess["disable"]            = &RenderProcess::disable;
      renderProcess["addParent"]          = sol::overload(PickOverload<RenderPass&>(&RenderProcess::addParent),
                                                          PickOverload<RenderProcess&>(&RenderProcess::addParent));
      renderProcess["addChild"]           = sol::overload(PickOverload<RenderPass&>(&RenderProcess::addChild),
                                                          PickOverload<RenderProcess&>(&RenderProcess::addChild));
      renderProcess["resizeBuffers"]      = &RenderProcess::resizeBuffers;
      renderProcess["recoverElapsedTime"] = &RenderProcess::recoverElapsedTime;
    }

    {
      auto ssrRenderProcess = state.new_usertype<ScreenSpaceReflectionsRenderProcess>("ScreenSpaceReflectionsRenderProcess",
                                                                                      sol::constructors<ScreenSpaceReflectionsRenderProcess(RenderGraph&)>(),
                                                                                      sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      ssrRenderProcess["setInputDepthBuffer"]        = &ScreenSpaceReflectionsRenderProcess::setInputDepthBuffer;
      ssrRenderProcess["setInputColorBuffer"]        = &ScreenSpaceReflectionsRenderProcess::setInputColorBuffer;
      ssrRenderProcess["setInputBlurredColorBuffer"] = &ScreenSpaceReflectionsRenderProcess::setInputBlurredColorBuffer;
      ssrRenderProcess["setInputNormalBuffer"]       = &ScreenSpaceReflectionsRenderProcess::setInputNormalBuffer;
      ssrRenderProcess["setInputSpecularBuffer"]     = &ScreenSpaceReflectionsRenderProcess::setInputSpecularBuffer;
      ssrRenderProcess["setOutputBuffer"]            = &ScreenSpaceReflectionsRenderProcess::setOutputBuffer;
    }

    {
      auto sobelRenderProcess = state.new_usertype<SobelFilterRenderProcess>("SobelFilterRenderProcess",
                                                                             sol::constructors<SobelFilterRenderProcess(RenderGraph&)>(),
                                                                             sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      sobelRenderProcess["setInputBuffer"]                   = &SobelFilterRenderProcess::setInputBuffer;
      sobelRenderProcess["setOutputGradientBuffer"]          = &SobelFilterRenderProcess::setOutputGradientBuffer;
      sobelRenderProcess["setOutputGradientDirectionBuffer"] = &SobelFilterRenderProcess::setOutputGradientDirectionBuffer;
    }

    {
      auto vignetteRenderProcess = state.new_usertype<VignetteRenderProcess>("VignetteRenderProcess",
                                                                             sol::constructors<VignetteRenderProcess(RenderGraph&)>(),
                                                                             sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      vignetteRenderProcess["setInputBuffer"]  = &VignetteRenderProcess::setInputBuffer;
      vignetteRenderProcess["setOutputBuffer"] = &VignetteRenderProcess::setOutputBuffer;
      vignetteRenderProcess["setStrength"]     = &VignetteRenderProcess::setStrength;
      vignetteRenderProcess["setOpacity"]      = &VignetteRenderProcess::setOpacity;
      vignetteRenderProcess["setColor"]        = &VignetteRenderProcess::setColor;
    }
  }
}

} // namespace Raz
