#include "PassCore.h"

#include "ImGUI.h"

namespace graphics
{
    PassData::PassData()
    {
        ImGuiPass::registerWindow([this] { imGUIRender(); });
    }

    void PassData::imGUIRender()
    {
        ImGui::Begin("Default Pass");
        imGUICommonParams();
        ImGui::End();
    }

    void PassData::imGUICommonParams()
    {
        if (ImGui::CollapsingHeader("Pass Constants"))
        {
            ImGui::DragFloat3("Eye Position", reinterpret_cast<float*>(&mPassConstants.eyePosition));

            ImGuiPass::matrix("View Matrix", mPassConstants.view);
            ImGuiPass::matrix("Inverse View Matrix", mPassConstants.invView);

            ImGuiPass::matrix("Projection Matrix", mPassConstants.projection);
            ImGuiPass::matrix("Inverse Projection Matrix", mPassConstants.invProjection);

            ImGuiPass::matrix("ViewProj Matrix", mPassConstants.viewProjection);
            ImGuiPass::matrix("Inverse ViewProj Matrix", mPassConstants.invViewProjection);

            ImGui::Text("Near Z: %.3f", static_cast<double>(mPassConstants.nearZ));
            ImGui::Text("Far Z: %.3f", static_cast<double>(mPassConstants.farZ));

            ImGui::Text("RT Size: %.3f, %.3f", static_cast<double>(mPassConstants.renderTargetSize.x), static_cast<double>(mPassConstants.renderTargetSize.y));
            ImGui::Text("Inv RT Size: %f, %f", static_cast<double>(mPassConstants.invRenderTargetSize.x), static_cast<double>(mPassConstants.invRenderTargetSize.y));
        }
    }

    void PassData::imGUISave()
    {
        if (ImGui::Button("SAVE"))
        {
            mIsDirty = true;
        }

        if (ImGui::Button("RESET"))
        {
            reset();
        }
    }
}
