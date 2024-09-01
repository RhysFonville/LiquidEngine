#include "PhysicalComponent.h"

void PhysicalComponent::tick(float dt) {
	physics_body.tick(dt);
	translate(physics_body.get_velocity() * dt);
	rotate(physics_body.get_angular_velocity() * dt);
}

void PhysicalComponent::render_editor_gui_section() {
	float mass = physics_body.get_mass();
	if (ImGui::InputFloat("Mass", &mass, ImGuiInputTextFlags_EnterReturnsTrue)) {
		physics_body.set_mass(mass);
	}

	FVector3 vel{physics_body.get_velocity()};
	float vec[3]{vel.x, vel.y, vel.z};
	if (ImGui::InputFloat3("Velocity", vec, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
		physics_body.set_velocity(vec);
	}

	FVector3 acc{physics_body.get_acceleration()};
	vec[0] = acc.x;
	vec[1] = acc.y;
	vec[2] = acc.z;
	if (ImGui::InputFloat3("Acceleration", vec, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
		physics_body.set_acceleration(vec);
	}

	FVector3 ang_vel{physics_body.get_angular_velocity()};
	vec[0] = ang_vel.x;
	vec[1] = ang_vel.y;
	vec[2] = ang_vel.z;
	if (ImGui::InputFloat3("Angular velocity", vec, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
		physics_body.set_angular_velocity(vec);
	}

	FVector3 ang_acc{physics_body.get_angular_acceleration()};
	vec[0] = ang_acc.x;
	vec[1] = ang_acc.y;
	vec[2] = ang_acc.z;
	if (ImGui::InputFloat3("Angular acceleration", vec, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
		physics_body.set_angular_acceleration(vec);
	}

	ImGui::Checkbox("Is frozen", &physics_body.frozen);

	render_physical_component_editor_gui_section();
}
