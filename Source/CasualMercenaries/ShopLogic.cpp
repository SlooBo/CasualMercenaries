// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ShopLogic.h"
#include "Inventory.h"
#include "Weapon.h"
#include "PlayerHud.h"
#include "Util.h"
#include "WeaponData.h"
#include "CMPlayerState.h"
#include "CMPlayerController.h"
UShopLogic::UShopLogic(const FObjectInitializer& PCIP)
{


}
UShopLogic::~UShopLogic()
{

}
void UShopLogic::SetUp(UUserWidget *shopWidget,UWorld *world)
{
	this->world = world;
	shopMenu = shopWidget;

	SetValueFromWidget(&upgradeButton1, "UpgradeButton1");
	upgradeButton1->OnClicked.AddDynamic(this, &UShopLogic::OnClickedUpgradeButton1);

	SetValueFromWidget(&upgradeButton2, "UpgradeButton2");
	upgradeButton2->OnClicked.AddDynamic(this, &UShopLogic::OnClickedUpgradeButton2);

	UButton* quitButton = nullptr;
	SetValueFromWidget(&quitButton, "QuitButton");
	quitButton->OnClicked.AddDynamic(this, &UShopLogic::OnClickedQuit);

	SetValueFromWidget(&buyButton, "BuyButton");
	buyButton->OnClicked.AddDynamic(this, &UShopLogic::OnClickedBuyButton);

	SetValueFromWidget(&buyButtonText, "BuyButtonText");

	SetValueFromWidget(&weaponImage, "SelectedWeaponImage");
	SetValueFromWidget(&descriptionTextBox, "DescriptionTextBox");
	SetValueFromWidget(&statTextBox, "StatText");

	SetValueFromWidget(&cashText, "CashText");

	UButton *tempWeaponButton = nullptr;
	for (int i = 0; i < 4; i++)
	{
		SetValueFromWidget<UButton>(&tempWeaponButton, "Weapon" + FString::FromInt(i));
		if (tempWeaponButton == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Error during shop creation!");
			return;
		}
		UWeaponSlot* weaponSlot = NewObject<UWeaponSlot>();
		weaponSlot->slotIndex = i;
		weaponSlot->shopLogic = this;
		tempWeaponButton->OnClicked.AddDynamic(weaponSlot, &UWeaponSlot::OnClicked);
		weaponSlotButtons.Add(tempWeaponButton);
		weaponSlots.Add(weaponSlot);
	}
	UButton *tempShopButton = nullptr;
	for (int i = 0; i < (uint32)WEAPONID::NO_WEAPON; i++)
	{
		SetValueFromWidget<UButton>(&tempShopButton, "ShopItem" + FString::FromInt(i));
		if (tempShopButton == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Not enough shop buttons!");
			continue;
		}
		UShopSlot* shopSlot = NewObject<UShopSlot>();
		shopSlot->slotIndex = i;
		shopSlot->shopLogic = this;

		tempShopButton->OnClicked.AddDynamic(shopSlot, &UShopSlot::OnClicked);
		shopSlotButtons.Add(tempShopButton);
		shopSlots.Add(shopSlot);
	}
	/*
	Setting buyButton's Tooltip widget
	*/
	/*UTextBlock *text = NewObject<UTextBlock>();
	text->SetText(FText::FromString("0 $"));
	text->Font.Size = 24;
	if (text != nullptr && buyButton != nullptr)
	buyButton->SetToolTip(text);
	*/
	UUserWidget *tooltipWidget = CreateWidget<UUserWidget>(world, Util::LoadObjFromPath<UClass>(TEXT("'/Game/Game/UI/ToolTip.ToolTip_C'")));
	if (tooltipWidget != nullptr && buyButton != nullptr)
	buyButton->SetToolTip(tooltipWidget);

	TArray<UWidget*> children;
	tooltipWidget->WidgetTree->GetAllWidgets(children);
	UTextBlock* tooltipTextTemp = nullptr;
	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		tooltipTextTemp = Cast<UTextBlock>(children[i]);
		if (tooltipTextTemp != nullptr)

		{
			this->tooltipText = tooltipTextTemp;
			break;
		}
	}




	OnClickedWeaponSlot(0);
}
void UShopLogic::OnClickedWeaponSlot(uint32 slotIndex)
{
	ChangeWeaponSlotColor(slotIndex, FLinearColor::Yellow);
	if (slotIndex != currentWeaponIndex)
		ChangeWeaponSlotColor(currentWeaponIndex, FLinearColor::White);
	currentWeaponIndex = slotIndex;
	AWeapon *weapon = Cast<ACMPlayerController>(world->GetFirstPlayerController())->GetInventory().GetWeapon(slotIndex);
	if (weapon != nullptr)
	{
		ChangeCurrentShopSlot((uint32)weapon->GetID());
	}
	//player has no weapon on this weapon slot
	else
	{
		upgradeButton1->SetIsEnabled(false);
		upgradeButton2->SetIsEnabled(false);
		buyButton->SetIsEnabled(false);
		FLinearColor color = FLinearColor(0.8f, 0.5f, 0.0f, 0.0f);
		FSlateColor slateColor = FSlateColor(color);
		ChangeShopSlotColor(currentShopIndex, slateColor);
		currentShopIndex = (uint32)WEAPONID::NO_WEAPON;
		UpdateInfoBox();
	}
}
void UShopLogic::OnClickedShopSlot(uint32 slotIndex)
{
	ChangeCurrentShopSlot(slotIndex);
}
UButton* UShopLogic::getShopButton(uint32 index)
{
	for (int i = 0; i < shopSlotButtons.Num(); i++)
	{
		FString *left = new FString();
		FString *right = new FString();
		bool success = shopSlotButtons[i]->GetName().Split("ShopItem", left, right);
		uint32 shopindex = FCString::Atoi(**right);
		if (index == shopindex)
		{
			return shopSlotButtons[i];
		}
	}
	return nullptr;
}
UButton* UShopLogic::getWeaponSlotButton(uint32 index)
{
	for (int i = 0; i < weaponSlotButtons.Num(); i++)
	{
		FString *left = new FString();
		FString *right = new FString();
		bool success = weaponSlotButtons[i]->GetName().Split("Weapon", left, right);
		uint32 slotindex = FCString::Atoi(**right);
		if (index == slotindex)
		{
			return weaponSlotButtons[i];
		}
	}
	return nullptr;
}
void UShopLogic::ChangeShopSlotColor(uint32 index, FSlateColor color)
{
	UButton* tempButton = getShopButton(index);
	if (tempButton != nullptr)
	{
		tempButton->WidgetStyle.Normal.TintColor = color;
		tempButton->WidgetStyle.Hovered.TintColor == color;
	}
}
void UShopLogic::ChangeWeaponSlotColor(uint32 index, FSlateColor color)
{
	UButton* tempButton = getWeaponSlotButton(index);
	if (tempButton != nullptr)
	{
		tempButton->WidgetStyle.Normal.TintColor = color;
	}
}
void UShopLogic::ChangeCurrentShopSlot(uint32 slotIndex)
{
	buyButton->SetIsEnabled(true);
	upgradeButton1->SetIsEnabled(true);
	upgradeButton2->SetIsEnabled(true);
	FLinearColor color = FLinearColor::Yellow;
	color = FLinearColor(0.8f, 0.5f, 0.0f, 1.0f);
	FSlateColor slateColor = FSlateColor(color);
	ChangeShopSlotColor(slotIndex, slateColor);
	if (slotIndex != currentShopIndex && currentShopIndex != -1 && currentShopIndex != (uint32)WEAPONID::NO_WEAPON)
	{
		color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		slateColor = FSlateColor(color);
		ChangeShopSlotColor(currentShopIndex, slateColor);
	}
	currentShopIndex = slotIndex;
	UpdateBuyButton();
	UpdateInfoBox();
}
void UShopLogic::OnClickedQuit()
{
	Cast<APlayerHud>(world->GetFirstPlayerController()->GetHUD())->changeUIElement(MenuType::GAME_UI);
}
void UShopLogic::OnClickedBuyButton()
{
	ACMPlayerController *player = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	player->BuyWeapon(currentWeaponIndex,AWeapon::GetIDFromInt(currentShopIndex));
	UpdateBuyButton();
}
void UShopLogic::UpdateBuyButton()
{
	//UTextBlock *text = NewObject<UTextBlock>();
	//text->SetText(FText::FromString("YAYAYAY"));
	//text->setSi
	//Cast<UTextBlock>(buyButton->ToolTipWidget)->Font.Size = 20;
	

	if (tooltipText == nullptr)
	{
		return;
	}
	//tooltipText->GetParent()->SetRenderTransformPivot(FVector2D(0.5f, -20.0f));
	//tooltipText->GetParent()->wid
		//UUserWidget *tooltipWidget = CreateWidget<UUserWidget>(world,  Util::LoadObjFromPath<UClass>(TEXT("'/Game/Game/UI/ToolTip.ToolTip_C'")));
	
	//buyButton->SetToolTipText(FText::FromString("Weapon: 1500 $ \n Weapon2: 200 $"));
	
	WEAPONID weaponid = AWeapon::GetIDFromInt((int8)currentShopIndex);
	FWeaponStruct *shopWeaponData = WeaponData::Get()->GetWeaponData(weaponid);

	ACMPlayerController *controller = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	if (controller == nullptr)
		return;
	ACMPlayerState *playerState = Cast<ACMPlayerState>(controller->PlayerState);
	if (playerState == nullptr)
		return;
	if (shopWeaponData!= nullptr)
	{
		
		AWeapon *weapon = Cast<ACMPlayerController>(world->GetFirstPlayerController())->GetInventory().GetWeapon(currentWeaponIndex);
		if (weapon == nullptr)
		{
			bool hasMoney = playerState->GetMoney() >= (int32)shopWeaponData->buyPrice;
				buyButton->WidgetStyle.Normal.TintColor = FSlateColor(hasMoney ? FLinearColor::Green: FLinearColor::Red);
		
			buyButtonText->SetText(FText::FromString(FString::FromInt(shopWeaponData->buyPrice) + " $"));
			tooltipText->SetText(FText::FromString("New weapon price: " + FString::FromInt(shopWeaponData->buyPrice)));
			UCanvasPanelSlot *canvas = Cast<UCanvasPanelSlot>(tooltipText->GetParent()->GetParent()->GetParent()->Slot);
			FVector2D size = canvas->GetSize();
			//canvas-
			canvas->Parent->RenderTransform.Translation = FVector2D(0.5f, size.Y * 2);;
			return;
		}
		FWeaponStruct *currentWeaponData = WeaponData::Get()->GetWeaponData(weapon->GetID());
		if (currentWeaponData != nullptr)
		{
			bool hasMoney = playerState->GetMoney() >= (int32)shopWeaponData->buyPrice - (int32)currentWeaponData->buyPrice / 2;
				buyButton->WidgetStyle.Normal.TintColor = FSlateColor(hasMoney ?FLinearColor::Green : FLinearColor::Red);
			
				int32 totalPrice = (int32)shopWeaponData->buyPrice - (int32)currentWeaponData->buyPrice / 2;
			buyButtonText->SetText(FText::FromString(FString::FromInt(totalPrice)+ " $"));
			
			tooltipText->SetText(FText::FromString("New weapon price: " + FString::FromInt(shopWeaponData->buyPrice) + "\n" + 
				"You will gain from old weapon: " + FString::FromInt(currentWeaponData->buyPrice / 2) + +"\n"+
				"Total price: " + FString::FromInt(shopWeaponData->buyPrice) + " - " + FString::FromInt(currentWeaponData->buyPrice/2) + 
				" = " + FString::FromInt(totalPrice)
				));
			tooltipText->GetParent()->RenderTransform.Translation = FVector2D(0.5f, tooltipText->GetParent()->GetDesiredSize().Y);
		}

		
	}

}
void UShopLogic::OnClickedUpgradeButton1()
{

	//todo upgrade
}
void UShopLogic::OnClickedUpgradeButton2()
{
	//todo upgrade
}
void UShopLogic::ChangeWeaponIconImage(UTexture2D *newImage)
{
	weaponImage->Brush.SetResourceObject(newImage);
}

void UShopLogic::UpdateInfoBox()
{
	WEAPONID weaponid = AWeapon::GetIDFromInt((int8)currentShopIndex);
	FWeaponStruct *currentWeaponData = WeaponData::Get()->GetWeaponData(weaponid);
	if (currentWeaponData != nullptr)
	{
		UTexture2D *test = Util::LoadObjFromPath<UTexture2D>(FName(*currentWeaponData->iconPath));
		ChangeWeaponIconImage(test);
		descriptionTextBox->SetText(FText::FromString(currentWeaponData->description));
		//	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice,
		//uint32 upgradePrice1, uint32 upgradePrice2, FString name, FString description, FString iconPath)
		
		FString statText =
			"ClipSize: " + FString::FromInt(currentWeaponData->clipSize) +
			"\nReload Time: " + FString::FromInt(currentWeaponData->reloadTime) +
			"\nDamage: " + FString::FromInt(currentWeaponData->damage) +
			"\nFire rate: " + FString::FromInt(currentWeaponData->fireRate) +
			"\nRange: " + WeaponData::Get()->GetRangeEnumString(currentWeaponData->range);
		statTextBox->SetText(FText::FromString(statText));
	}
}

void UShopLogic::Update()
{
	ACMPlayerController *controller = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	ACMPlayerState *playerState = Cast<ACMPlayerState>(controller->PlayerState);
	cashText->SetText(FText::FromString(FString::FromInt(playerState->GetMoney()) + " $"));

	
	if (controller->GetInventory().currentWeapon != currentWeaponIndex)
		OnClickedWeaponSlot(controller->GetInventory().currentWeapon);
	
}