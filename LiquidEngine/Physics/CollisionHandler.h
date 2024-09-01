#pragma once

#include "../Components/PhysicalComponent.h"

struct OverlapEventInfo {
	bool overlapped{false};
	
	struct OverlapInfo {
		FVector3 point_of_overlap{};
		FVector3 axis_overlap{};
	} overlap_info;

	struct OverlapEventObject {
		PhysicalComponent* component{nullptr};
	};
	std::pair<OverlapEventObject, OverlapEventObject> objects{std::make_pair(OverlapEventObject{}, OverlapEventObject{})};
};

class CollisionHandler {
public:
	CollisionHandler() { }

	void handle_collision(const OverlapEventInfo &info) noexcept;
};
