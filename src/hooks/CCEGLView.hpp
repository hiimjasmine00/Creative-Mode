#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include "../Hover.hpp"

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS

class $modify(CCEGLView) {
    void onGLFWMouseMoveCallBack(GLFWwindow* w, double x, double y) {
        CCEGLView::onGLFWMouseMoveCallBack(w, x, y);
        Hover::get()->updateHover(getMousePos());
    }
};

#endif