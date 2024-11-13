#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWaterInteractable.generated.h"

UINTERFACE(MinimalAPI)
class UWaterInteractable : public UInterface
{
	GENERATED_BODY()
};

class GP27_TTC_API IWaterInteractable
{
	GENERATED_BODY()

public:
	bool DeleteWhenInWater() const { return m_bDeleteWhenInWater; }
	bool DoSplash() const { return m_bDoWaterSplash; }
	FVector GetSplashScale() const { return FVector(m_SplashFactor); }
	virtual void SetIsDecomposing(bool isDecomposing){m_bIsDecomposing = isDecomposing;}
	virtual void OnDeletion() {}
protected:
	//Deletion
	bool m_bDeleteWhenInWater{true};
	bool m_bIsDecomposing{false};
	float m_TimeInWater{0.5f};


	//Splash
	bool m_bDoWaterSplash{true};
	float m_SplashFactor{1.f};
	
};
