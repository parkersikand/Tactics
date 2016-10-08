#pragma once


#include "System.h"
#include "Entity.h"
#include "Event.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace Tactics {

	namespace Events {

		struct BaseKeyEvent : public ECS::Event {
			// ascii code of the pressed key
			int keyCode;

			// modifiers
			int mods;
		};

		struct KeyDown : public BaseKeyEvent {};
		struct KeyUp : public BaseKeyEvent {};
		struct KeyRepeat : public BaseKeyEvent {};

		struct ScrollEvent : public ECS::Event {
			double x;
			double y;
		};
	}


	class WindowManager : public ECS::RunnableSystem {
	public:

		WindowManager() {
			myWindow = WindowManager::initWindow();
		};
		WindowManager(std::string title) {
			myWindow = WindowManager::initWindow(title);
		}
		WindowManager(std::string title, int w, int h) {
			myWindow = WindowManager::initWindow(title, w, h);
		}
		WindowManager(GLFWwindow * w) : myWindow(w) {};

		static GLFWwindow * initWindow(std::string = "ECS::Window v0.0dev", int w = 1072, int h = 768);

		// check window events, handle closing window, etc
		virtual void run(ECS::RunnableWorld &);

		GLFWwindow * getWindow() const {
			return myWindow;
		}

		// add drawing system
		virtual void onRegister(ECS::RunnableWorld &);

		static void _key_handle(GLFWwindow* window, int key, int scancode, int action, int mods) {
			switch (action) {
			case GLFW_PRESS: {
				Events::KeyDown kde;
				kde.keyCode = key;
				kde.mods = mods;
				ECS::EventDispatcher::postEvent(kde);
				break;
			}
			case GLFW_RELEASE: {
				Events::KeyUp kue;
				kue.keyCode = key;
				kue.mods = mods;
				ECS::EventDispatcher::postEvent(kue);
				break;
			}
			case GLFW_REPEAT: {
				Events::KeyRepeat kre;
				kre.keyCode = key;
				kre.mods = mods;
				ECS::EventDispatcher::postEvent(kre);
				break;
			}
			} // case
		} // void _key_handle

		 // TODO implement these callbacks at the global level and delegate down to events
		static void _handleScroll(GLFWwindow * window, double x, double y) {
			Events::ScrollEvent scrollEvent;
			scrollEvent.x = x;
			scrollEvent.y = y;
			ECS::EventDispatcher::postEvent(scrollEvent);
		}

	private:
		GLFWwindow * myWindow;
		
	}; // WindowManager


}

