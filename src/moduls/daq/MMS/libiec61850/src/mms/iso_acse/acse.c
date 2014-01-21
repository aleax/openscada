/*
 *  acse.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *  This file is part of libIEC61850.
 *
 *  libIEC61850 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libIEC61850 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "acse.h"

#include "ber_encoder.h"
#include "ber_decode.h"

static uint8_t appContextNameMms[] = { 0x28, 0xca, 0x22, 0x02, 0x03 };

static uint8_t apTitle_1_1_1_999_1[] = { 0x29, 0x01, 0x87, 0x67, 0x01 };

static uint8_t berOid[2] = { 0x51, 0x01 };

static uint8_t auth_mech_password_oid[] = { 0x52, 0x03, 0x01 };

static uint8_t requirements_authentication[] = { 0x80 };

static bool
checkAuthMechanismName(AcseConnection* self, uint8_t* authMechanism, int authMechLen)
{
    if (authMechanism != NULL ) {

        if (self->authentication->mechanism == AUTH_PASSWORD) {

            if (authMechLen != 3)
                return false;

            if (memcmp(auth_mech_password_oid, authMechanism, 3) == 0)
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

static bool
checkAuthenticationValue(AcseConnection* self, uint8_t* authValue, int authValueLen)
{
    if (authValue == NULL )
        return false;

    if (self->authentication->mechanism == AUTH_PASSWORD) {
		if (authValueLen != strlen(self->authentication->value.password.string))
			return false;

		if (memcmp(authValue, self->authentication->value.password.string, authValueLen) != 0)
			return false;

		return true;
    }

    return false;
}

static bool
checkAuthentication(AcseConnection* self, uint8_t* authMechanism, int authMechLen, uint8_t* authValue, int authValueLen)
{
    if (self->authentication != NULL ) {
        if (!checkAuthMechanismName(self, authMechanism, authMechLen))
            return false;

        return checkAuthenticationValue(self, authValue, authValueLen);
    }
    else
        return true;
}



static int
parseUserInformation(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos, bool* userInfoValid)
{
	if (DEBUG) printf("ACSE: parseUserInformation %i %i\n", bufPos, maxBufPos);

	bool hasindirectReference = false;
	bool isBer = true; /* isBer defaults to true if no direct-reference tag is present */
	bool isDataValid = false;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int len;

		bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

		switch (tag) {
		case 0x06: /* direct-reference */
			isBer = false;

			if (len == 2) {
				if (memcmp(buffer + bufPos, berOid, 2) == 0) {
					isBer = true;
				}
			}

			bufPos += len;

			break;

		case 0x02: /* indirect-reference */
			self->nextReference = BerDecoder_decodeUint32(buffer, len, bufPos);
			bufPos += len;
			hasindirectReference = true;
			break;

		case 0xa0: /* encoding */
			isDataValid = true;

			self->userDataBufferSize = len;
			self->userDataBuffer = buffer + bufPos;

			bufPos += len;

			break;

		default: /* ignore unknown tag */
			bufPos += len;
		}
	}


	if (DEBUG) {
		if (!isBer) printf("ACSE: User data is not BER!\n");

		if (!hasindirectReference) printf("ACSE: User data has no indirect reference!\n");

		if (!isDataValid) printf("ACSE: No valid user data\n");
	}

	if (isBer && hasindirectReference && isDataValid)
		*userInfoValid = true;
	else
		*userInfoValid = false;

	return bufPos;
}

static AcseIndication
parseAarePdu(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos)
{
	if (DEBUG) printf("ACSE: parse AARE PDU\n");

	bool userInfoValid;

	uint32_t result = 99;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int len;

		bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

		switch (tag) {
		case 0xa1: /* application context name */
			bufPos += len;
			break;

		case 0xa2: /* result */
			bufPos++;

			bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
			result = BerDecoder_decodeUint32(buffer, len, bufPos);

			bufPos += len;
			break;

		case 0xa3: /* result source diagnostic */
			bufPos += len;
			break;

		case 0xbe: /* user information */
			if (buffer[bufPos]  != 0x28) {
				if (DEBUG) printf("ACSE: invalid user info\n");
				bufPos += len;
			}
			else {
				bufPos++;

				bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

				bufPos = parseUserInformation(self, buffer, bufPos, bufPos + len, &userInfoValid);
			}
			break;

		default: /* ignore unknown tag */
		    if (DEBUG)
		        printf("parseAarePdu: unknown tag %02x\n", tag);

			bufPos += len;
			break;
		}
	}

	if (!userInfoValid)
		return ACSE_ERROR;

	if (result != 0)
		return ACSE_ASSOCIATE_FAILED;

    return ACSE_ASSOCIATE;
}

static AcseIndication
parseAarqPdu(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos)
{
	if (DEBUG) printf("ACSE: parse AARQ PDU\n");

	uint8_t* authValue = NULL;
	int authValueLen = 0;
	uint8_t* authMechanism = NULL;
	int authMechLen = 0;
	bool userInfoValid = false;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int len;

		bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

		if (bufPos < 0) {
		    if (DEBUG)
	            printf("parseAarqPdu: user info invalid!\n");
	        return ACSE_ASSOCIATE_FAILED;
		}

		switch (tag) {
		case 0xa1: /* application context name */
			bufPos += len;
			break;

		case 0xa2: /* called AP title */
			bufPos += len;
			break;
		case 0xa3: /* called AE qualifier */
			bufPos += len;
			break;

		case 0xa6: /* calling AP title */
			bufPos += len;
			break;

		case 0xa7: /* calling AE qualifier */
			bufPos += len;
			break;

		case 0x8a: /* sender ACSE requirements */
            bufPos += len;
			break;

		case 0x8b: /* (authentication) mechanism name */
			authMechLen = len;
			authMechanism = buffer + bufPos;
			bufPos += len;
			break;

		case 0xac: /* authentication value */
			bufPos++;
			bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
			authValueLen = len;
			authValue = buffer + bufPos;
			bufPos += len;
			break;

		case 0xbe: /* user information */
			if (buffer[bufPos]  != 0x28) {
				if (DEBUG) printf("ACSE: invalid user info\n");
				bufPos += len;
			}
			else {
				bufPos++;

				bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

				bufPos = parseUserInformation(self, buffer, bufPos, bufPos + len, &userInfoValid);
			}
			break;

		default: /* ignore unknown tag */
		    if (DEBUG)
		        printf("parseAarqPdu: unknown tag %02x\n", tag);

			bufPos += len;
			break;
		}
	}

    if (checkAuthentication(self, authMechanism, authMechLen, authValue, authValueLen) == false) {
        if (DEBUG)
            printf("parseAarqPdu: check authentication failed!\n");

        return ACSE_ASSOCIATE_FAILED;
    }

    if (userInfoValid == false) {
        if (DEBUG)
            printf("parseAarqPdu: user info invalid!\n");

    	return ACSE_ASSOCIATE_FAILED;
    }

    return ACSE_ASSOCIATE;
}

void
AcseConnection_init(AcseConnection* self)
{
    self->state = idle;
    self->nextReference = 0;
    self->userDataBuffer = NULL;
    self->userDataBufferSize = 0;
    self->authentication = NULL;
}

void
AcseConnection_setAuthenticationParameter(AcseConnection* self, AcseAuthenticationParameter auth)
{
    self->authentication = auth;
}

void
AcseConnection_destroy(AcseConnection* connection)
{
}

AcseIndication
AcseConnection_parseMessage(AcseConnection* self, ByteBuffer* message)
{
    AcseIndication indication;

    uint8_t* buffer = message->buffer;

    int messageSize = message->size;

    int bufPos = 0;

    uint8_t messageType = buffer[bufPos++];

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, messageSize);

    if (bufPos < 0) {
        printf("AcseConnection_parseMessage: invalid ACSE message!\n");

        return ACSE_ERROR;
    }

    switch (messageType) {
    case 0x60:
    	indication = parseAarqPdu(self, buffer, bufPos, messageSize);
    	break;
    case 0x61:
    	indication = parseAarePdu(self, buffer, bufPos, messageSize);
		break;
    case 0x62: /* A_RELEASE.request RLRQ-apdu */
        indication = ACSE_RELEASE_REQUEST;
        break;
    case 0x63: /* A_RELEASE.response RLRE-apdu */
        indication = ACSE_RELEASE_RESPONSE;
        break;
    case 0x64: /* A_ABORT */
        indication = ACSE_ABORT;
        break;
    default:
    	if (DEBUG) printf("ACSE: Unknown ACSE message\n");
    	indication = ACSE_ERROR;
    	break;
    }

    return indication;
}

void
AcseConnection_createAssociateFailedMessage(AcseConnection* self, ByteBuffer* writeBuffer)
{
	AcseConnection_createAssociateResponseMessage(self, ACSE_RESULT_REJECT_PERMANENT, writeBuffer, NULL);
}

void
AcseConnection_createAssociateResponseMessage(AcseConnection* self,
		uint8_t acseResult,
        ByteBuffer* writeBuffer,
        ByteBuffer* payload
        )
{
	int appContextLength = 9;
	int resultLength = 5;
	int resultDiagnosticLength = 5;

	int fixedContentLength = appContextLength + resultLength + resultDiagnosticLength;

	int variableContentLength = 0;

	int payloadLength;
	int assocDataLength;
	int userInfoLength;
	int nextRefLength;

	if (payload != NULL) {
		payloadLength = payload->size;

		/* single ASN1 type tag */
		variableContentLength += payloadLength;
		variableContentLength += 1;
		variableContentLength += BerEncoder_determineLengthSize(payloadLength);

		/* indirect reference */
		nextRefLength = BerEncoder_UInt32determineEncodedSize(self->nextReference);
		variableContentLength += nextRefLength;
		variableContentLength += 2;

		/* direct-reference BER */
		variableContentLength += 4;

		/* association data */
		assocDataLength = variableContentLength;
		variableContentLength += BerEncoder_determineLengthSize(assocDataLength);
		variableContentLength += 1;

		/* user information */
		userInfoLength = variableContentLength;
		variableContentLength += BerEncoder_determineLengthSize(userInfoLength);
		variableContentLength += 1;

		variableContentLength += 2;
	}

	int contentLength = fixedContentLength + variableContentLength;

	uint8_t* buffer = writeBuffer->buffer;
	int bufPos = 0;

	bufPos = BerEncoder_encodeTL(0x61, contentLength, buffer, bufPos);

	/* application context name */
	bufPos = BerEncoder_encodeTL(0xa1, 7, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
	memcpy(buffer + bufPos, appContextNameMms, 5);
	bufPos += 5;

	/* result */
	bufPos = BerEncoder_encodeTL(0xa2, 3, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
	buffer[bufPos++] = acseResult;

	/* result source diagnostics */
	bufPos = BerEncoder_encodeTL(0xa3, 5, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0xa1, 3, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
	buffer[bufPos++] = 0;

	if (payload != NULL) {
		/* user information */
		bufPos = BerEncoder_encodeTL(0xbe, userInfoLength, buffer, bufPos);

		/* association data */
		bufPos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, bufPos);

		/* direct-reference BER */
		bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
		buffer[bufPos++] = berOid[0];
		buffer[bufPos++] = berOid[1];

		/* indirect-reference */
		bufPos = BerEncoder_encodeTL(0x02, nextRefLength, buffer, bufPos);
		bufPos = BerEncoder_encodeUInt32(self->nextReference, buffer, bufPos);

		/* single ASN1 type */
		bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);
		memcpy(buffer + bufPos, payload->buffer, payloadLength);
		bufPos += payloadLength;
	}

	writeBuffer->size = bufPos;
}

void
AcseConnection_createAssociateResponseMessageBC(AcseConnection* self,
        uint8_t acseResult,
        BufferChain writeBuffer,
        BufferChain payload
        )
{
    int appContextLength = 9;
    int resultLength = 5;
    int resultDiagnosticLength = 5;

    int fixedContentLength = appContextLength + resultLength + resultDiagnosticLength;

    int variableContentLength = 0;

    int payloadLength;
    int assocDataLength;
    int userInfoLength;
    int nextRefLength;

    if (payload != NULL) {
        payloadLength = payload->length;

        /* single ASN1 type tag */
        variableContentLength += payloadLength;
        variableContentLength += 1;
        variableContentLength += BerEncoder_determineLengthSize(payloadLength);

        /* indirect reference */
        nextRefLength = BerEncoder_UInt32determineEncodedSize(self->nextReference);
        variableContentLength += nextRefLength;
        variableContentLength += 2;

        /* direct-reference BER */
        variableContentLength += 4;

        /* association data */
        assocDataLength = variableContentLength;
        variableContentLength += BerEncoder_determineLengthSize(assocDataLength);
        variableContentLength += 1;

        /* user information */
        userInfoLength = variableContentLength;
        variableContentLength += BerEncoder_determineLengthSize(userInfoLength);
        variableContentLength += 1;

        variableContentLength += 2;
    }

    int contentLength = fixedContentLength + variableContentLength;

    //TODO check if buffer is large enough!
    //if (writeBuffer->partMaxLength >= (contentLength - payloadLength))

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x61, contentLength, buffer, bufPos);

    /* application context name */
    bufPos = BerEncoder_encodeTL(0xa1, 7, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
    memcpy(buffer + bufPos, appContextNameMms, 5);
    bufPos += 5;

    /* result */
    bufPos = BerEncoder_encodeTL(0xa2, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = acseResult;

    /* result source diagnostics */
    bufPos = BerEncoder_encodeTL(0xa3, 5, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa1, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 0;

    if (payload != NULL) {
        /* user information */
        bufPos = BerEncoder_encodeTL(0xbe, userInfoLength, buffer, bufPos);

        /* association data */
        bufPos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, bufPos);

        /* direct-reference BER */
        bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
        buffer[bufPos++] = berOid[0];
        buffer[bufPos++] = berOid[1];

        /* indirect-reference */
        bufPos = BerEncoder_encodeTL(0x02, nextRefLength, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32(self->nextReference, buffer, bufPos);

        /* single ASN1 type */
        bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);
    }

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payloadLength;
    writeBuffer->nextPart = payload;
}

void
AcseConnection_createAssociateRequestMessage(AcseConnection* self,
        ByteBuffer* writeBuffer,
        ByteBuffer* payload)
{
	int payloadLength = payload->size;
	int authValueLength;
	int authValueStringLength;

	int passwordLength;

	int contentLength = 0;

	/* application context name */
	contentLength += 9;

	/* called AP title */
	contentLength += 9;

	/* called AP qualifier */
	contentLength += 5;

    /* calling AP title */
	contentLength += 8;

	/* calling AP qualifier */
	contentLength += 5;

	if (self->authentication != NULL) {

		/* sender ACSE requirements */
		contentLength += 4;

		/* mechanism name */
		contentLength += 5;

		/* authentication value */
		if (self->authentication->mechanism == AUTH_PASSWORD) {
			contentLength += 2;
			passwordLength = strlen(self->authentication->value.password.string);

			authValueStringLength = BerEncoder_determineLengthSize(passwordLength);

			contentLength += passwordLength  + authValueStringLength;

			authValueLength = BerEncoder_determineLengthSize(passwordLength + authValueStringLength + 1);

			contentLength += authValueLength;
		}
		else {
			contentLength += 2;
		}
	}

	/* user information */
	int userInfoLength = 0;

	/* single ASN1 type tag */
	userInfoLength += payloadLength;
	userInfoLength += 1;
	userInfoLength += BerEncoder_determineLengthSize(payloadLength);

	/* indirect reference */
	userInfoLength += 1;
	userInfoLength += 2;

	/* direct-reference BER */
	userInfoLength += 4;

	/* association data */
	int assocDataLength = userInfoLength;
	userInfoLength += BerEncoder_determineLengthSize(assocDataLength);
	userInfoLength += 1;

	/* user information */
	int userInfoLen = userInfoLength;
	userInfoLength += BerEncoder_determineLengthSize(userInfoLength);
	userInfoLength += 1;

	//userInfoLength += 2;

	contentLength += userInfoLength;

	//contentLength += 3; /* ??? */

	uint8_t* buffer = writeBuffer->buffer;
	int bufPos = 0;

	bufPos = BerEncoder_encodeTL(0x60, contentLength, buffer, bufPos);

	/* application context name */
	bufPos = BerEncoder_encodeTL(0xa1, 7, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
	memcpy(buffer + bufPos, appContextNameMms, 5);
	bufPos += 5;

	/* called AP title */
	bufPos = BerEncoder_encodeTL(0xa2, 7, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
	memcpy(buffer + bufPos, apTitle_1_1_1_999_1, 5);
	bufPos += 5;

	/* called AE qualifier */
	bufPos = BerEncoder_encodeTL(0xa3, 3, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
	buffer[bufPos++] = 0x0c;

	/* calling AP title */
	bufPos = BerEncoder_encodeTL(0xa6, 6, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x06, 4, buffer, bufPos);
	memcpy(buffer + bufPos, apTitle_1_1_1_999_1, 4);
	bufPos += 4;

	/* calling AE qualifier */
	bufPos = BerEncoder_encodeTL(0xa7, 3, buffer, bufPos);
	bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
	buffer[bufPos++] = 0x0c;

	if (self->authentication != NULL) {
		/* sender requirements */
		bufPos = BerEncoder_encodeTL(0x8a, 2, buffer, bufPos);
		buffer[bufPos++] = 0x04;

		if (self->authentication->mechanism == AUTH_PASSWORD) {
		    buffer[bufPos++] = requirements_authentication[0];

			bufPos = BerEncoder_encodeTL(0x8b, 3, buffer, bufPos);
			memcpy(buffer + bufPos, auth_mech_password_oid, 3);
			bufPos += 3;

			/* authentication value */
			bufPos = BerEncoder_encodeTL(0xac, authValueStringLength + passwordLength + 1, buffer, bufPos);
			bufPos = BerEncoder_encodeTL(0x80, passwordLength, buffer, bufPos);
			memcpy(buffer + bufPos, self->authentication->value.password.string, passwordLength);
			bufPos += passwordLength;
		}
		else { /* AUTH_NONE */
		    buffer[bufPos++] = 0;
		}
	}

	if (payload != NULL) {
		/* user information */
		bufPos = BerEncoder_encodeTL(0xbe, userInfoLen, buffer, bufPos);

		/* association data */
		bufPos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, bufPos);

		/* direct-reference BER */
		bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
		buffer[bufPos++] = berOid[0];
		buffer[bufPos++] = berOid[1];

		/* indirect-reference */
		bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
		buffer[bufPos++] = 3;
		//bufPos = BerEncoder_encodeUInt32(3, buffer, bufPos);

		/* single ASN1 type */
		bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);
		memcpy(buffer + bufPos, payload->buffer, payloadLength);
		bufPos += payloadLength;
	}

	writeBuffer->size = bufPos;
}

void
AcseConnection_createAbortMessage(AcseConnection* self, ByteBuffer* writeBuffer, bool isProvider)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x61;
    buffer[1] = 3;
    buffer[2] = 80;
    buffer[3] = 1;

    if (isProvider)
        buffer[4] = 1;
    else
        buffer[4] = 0;
}

void
AcseConnection_createReleaseRequestMessage(AcseConnection* self, ByteBuffer* writeBuffer)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x62;
    buffer[1] = 0;
}

void
AcseConnection_createReleaseResponseMessage(AcseConnection* self, ByteBuffer* writeBuffer)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x63;
    buffer[1] = 0;
}

