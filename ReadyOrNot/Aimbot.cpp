#include "Cheats.h"
#include "Utils.h"

bool AimbotEnabled = false;

void Cheats::ToggleAimbot() {
	AimbotEnabled = !AimbotEnabled;
	std::cout << "Aimbot: " << (AimbotEnabled ? "ON" : "OFF") << "\n";
}

void Cheats::Aimbot(Variables* Vars)
{
	if (!AimbotEnabled) return;

	if (!Vars || !Vars->PlayerController || !Vars->ReadyOrNotChar) return;

	ULevel* Level = Vars->Level;
	if (!Level || !Level->Actors) return;

	for (int i = 0; i < Level->Actors.Num(); i++)
	{
		AActor* Actor = Level->Actors[i];

		if (!Actor || Actor == Vars->Character) continue;
		if (!Actor->Name.ToString().contains("SuspectController")) continue;

		ASuspectController* TargetController = (ASuspectController*)Actor;
		if (!TargetController) continue;
		ASuspectCharacter* TargetChar;
		if (TargetController->Pawn)
		{
			TargetChar = (ASuspectCharacter*)TargetController->Pawn;

		}
		else
			continue;

		if (TargetChar && !TargetChar->IsDeadOrUnconscious() && !TargetChar->IsOnSWATTeam())
		{
			FVector TargetLocation = TargetChar->K2_GetActorLocation();
			FVector PlayerLocation = Vars->Character->K2_GetActorLocation();
			FVector Direction = (TargetLocation - PlayerLocation).GetNormalized();
			FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(Direction);
			Vars->PlayerController->SetControlRotation(TargetRotation);
			break; // Aim at the first valid target found
		}
	}
}