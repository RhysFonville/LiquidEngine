<h2>Template Behavior Class:</h2>

<h3>main.cpp</h3>

```
#include "Engine.h"
#include "MyBehavior.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	Object obj;
	obj.transform.position = { 0.0f, 0.0f, 5.0f };
	obj.transform.rotation = { 70.0f, 70.0f, 70.0f };
	obj.transform.scale = { 1, 1, 1 };
	engine.scene.objects.get()->push_back(obj);

	Object obj1;
	obj1.transform.position = { 3.0f, 1.0f, 7.0f };
	obj1.transform.rotation = { 0.0f, 0.0f, 0.0f };
	obj1.transform.scale = { 1, 1, 1 };
	engine.scene.objects.get()->push_back(obj1);

	engine.scene.behavior_manager.behaviors.emplace_back(
		std::shared_ptr<ObjectBehavior>(new MyBehavior((*engine.scene.objects)[0])));
	engine.scene.behavior_manager.behaviors.emplace_back(
		std::shared_ptr<ObjectBehavior>(new MyBehavior((*engine.scene.objects)[1])));

	engine.loop();
}
```

<h3>MyBehavior.h</h3>

```
#pragma once

#include "ObjectBehavior.h"

class MyBehavior : public ObjectBehavior {
public:
	MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object);

	void tick() override;
```

<h3>MyBehavior.cpp</h3>

```
#include "MyBehavior.h"

MyBehavior::MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object)
	: ObjectBehavior(objects, object, "MyBehavior") { }

void MyBehavior::tick() {
	object->rotate({ 0.01f, 0.01f, 0.01f });
}

};


```
