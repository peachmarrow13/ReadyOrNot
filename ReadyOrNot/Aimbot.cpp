#include "Cheats.h"
#include "Utils.h"

bool AimbotEnabled = false;

void Cheats::ToggleAimbot() {
	std::cout << "Aimbot Toggled\n";
	AimbotEnabled = !AimbotEnabled;
	std::cout << "Aimbot: " << (AimbotEnabled ? "ON" : "OFF") << "\n";
}

void Cheats::Aimbot()
{
	if (!AimbotEnabled) return;
	Variables* Vars = Utils::GetVariables();
	if (!Vars || !Vars->PlayerController || !Vars->ReadyOrNotChar) return;
	//FRotator PlayerRotation = Vars->PlayerController->ControlRotation;
	APlayerController* PlayerController = Vars->PlayerController;
	ULevel* Level = Vars->Level;
	if (!Level || !Level->Actors) return;
	for (int i = 0; i < Level->Actors.Num(); i++)
	{
		AActor* Actor = Level->Actors[i];
		if (!Actor || Actor == Vars->Character) continue;
		if (!Actor->Name.ToString().contains("Suspect")) continue;
		AReadyOrNotCharacter* TargetChar = (AReadyOrNotCharacter*)Actor;
		std::cout << "Aimbot\n";
		if (TargetChar && !TargetChar->IsDeadOrUnconscious() && !TargetChar->IsOnSWATTeam())
		{
			std::cout << "Aimbot\n";
			//FVector TargetLocation = TargetChar->K2_GetActorLocation();
			//FVector PlayerLocation = Vars->Character->K2_GetActorLocation();
			//FVector Direction = (TargetLocation - PlayerLocation).GetNormalized();
			//FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(Direction);
			//Vars->PlayerController->SetControlRotation(TargetRotation);
			//std::cout << "Aimbot aimed at target: " << Actor->Name.ToString() << "\n";
			//std::cout << "From: " << PlayerRotation.Pitch << ", " << PlayerRotation.Yaw << ", " << PlayerRotation.Roll << "\n";
			break; // Aim at the first valid target found
		}
	}
}