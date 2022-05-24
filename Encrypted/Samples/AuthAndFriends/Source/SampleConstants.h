// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

struct SampleConstants
{
	/** The product id for the running application, found on the dev portal */
	static constexpr char ProductId[] = "02c0d26893eb442095e9cbd2c47e2e24";

	/** The sandbox id for the running application, found on the dev portal */
	static constexpr char SandboxId[] = "443496df21234596bbef53b2a4952dcf";

	/** The deployment id for the running application, found on the dev portal */
	static constexpr char DeploymentId[] = "d5dc5ddf91814094ad8166a08a9ff143";

	/** Client id of the service permissions entry, found on the dev portal */
	static constexpr char ClientCredentialsId[] = "xyza7891u8FeD7uR3Kc5B5oeAWoE4bkO";

	/** Client secret for accessing the set of permissions, found on the dev portal */
	static constexpr char ClientCredentialsSecret[] = "hs6ODzDI8dBAD5d2A2aoNj46i4+tR9tOJDwYhcuX5L0";

	/** Game name */
	static constexpr char GameName[] = "EOSTest";

	/** Encryption key. Not used by this sample. */
	static constexpr char EncryptionKey[] = "1111111111111111111111111111111111111111111111111111111111111111";
};