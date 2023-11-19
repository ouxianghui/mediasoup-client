/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <random>
#include <limits>
#include "json/jsonable.hpp"
#include "absl/types/optional.h"
#include "json.hpp"

namespace {

int32_t generateID() {
    std::random_device sd;
    std::minstd_rand linear(sd());
    std::uniform_int_distribution<int32_t>dist(0, std::numeric_limits<int32_t>::max());
    return dist(linear);
}

}

namespace signaling {

struct MessageType {
    absl::optional<bool> request;
    absl::optional<bool> response;
    absl::optional<bool> notification;
    FIELDS_MAP("request", request, "response", response, "notification", notification);
};

struct RequestHeader {
    absl::optional<bool> request;
    absl::optional<int64_t> id;
    absl::optional<std::string> method;
    FIELDS_MAP("request", request, "id", id, "method", method);
};

struct ResponseHeader {
    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    FIELDS_MAP("response", response, "id", id, "ok", ok);
};

struct NotificationHeader {
    absl::optional<bool> notification;
    absl::optional<std::string> method;
    FIELDS_MAP("notification", notification, "method", method);
};

struct BasicResponse {
    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    FIELDS_MAP("response", response, "id", id, "ok", ok);
};

struct GetRouterRtpCapabilitiesRequest {
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "getRouterRtpCapabilities";
    FIELDS_MAP("request", request, "id", id, "method", method);
};

struct GetRouterRtpCapabilitiesResponse {
    struct RTCPFeedback {
        absl::optional<std::string> type;
        absl::optional<std::string> parameter;
        FIELDS_MAP("type", type, "parameter", parameter);
    };

    struct Parameters {
        absl::optional<int32_t> apt;
        absl::optional<int32_t> levelAsymmetryAllowed;
        absl::optional<int32_t> packetizationMode;
        absl::optional<std::string> profileLevelId;
        absl::optional<int32_t> xGoogleStartBitrate;
        FIELDS_MAP("apt", apt, "level-asymmetry-allowed", levelAsymmetryAllowed, "packetization-mode", packetizationMode, "profile-level-id", profileLevelId, "x-google-start-bitrate", xGoogleStartBitrate);
    };

    struct Codec {
        absl::optional<std::string> kind;
        absl::optional<std::string> mimeType;
        absl::optional<int32_t> clockRate;
        absl::optional<int32_t> channels;
        absl::optional<std::vector<RTCPFeedback>> rtcpFeedback;
        absl::optional<Parameters> parameters;
        absl::optional<int32_t> preferredPayloadType;
        FIELDS_MAP("kind", kind, "mimeType", mimeType, "clockRate", clockRate, "channels", channels, "rtcpFeedback", rtcpFeedback, "parameters", parameters, "preferredPayloadType", preferredPayloadType);
    };

    struct HeaderExtension {
        absl::optional<std::string> kind;
        absl::optional<std::string> uri;
        absl::optional<int32_t> preferredId;
        absl::optional<bool> preferredEncrypt;
        absl::optional<std::string> direction;
        FIELDS_MAP("kind", kind, "uri", uri, "preferredId", preferredId, "preferredEncrypt", preferredEncrypt, "direction", direction);
    };

    struct Data {
        absl::optional<std::vector<Codec>> codecs;
        absl::optional<std::vector<HeaderExtension>> headerExtensions;
        FIELDS_MAP("codecs", codecs, "headerExtensions", headerExtensions);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct CreateWebRtcTransportRequest {
    struct NumStreams {
        absl::optional<int32_t> MIS;
        absl::optional<int32_t> OS;
        FIELDS_MAP("MIS", MIS, "OS", OS);
    };

    struct SCTPCapabilities {
        absl::optional<NumStreams> numStreams;
        FIELDS_MAP("numStreams", numStreams);
    };

    struct Data {
        absl::optional<bool> consuming;
        absl::optional<bool> producing;
        absl::optional<bool> forceTcp;
        absl::optional<SCTPCapabilities> sctpCapabilities;
        FIELDS_MAP("consuming", consuming, "producing", producing, "forceTcp", forceTcp, "sctpCapabilities", sctpCapabilities);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "createWebRtcTransport";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct CreateWebRtcTransportResponse {
    struct Fingerprint {
        absl::optional<std::string> algorithm;
        absl::optional<std::string> value;
        FIELDS_MAP("algorithm", algorithm, "value", value);
    };

    struct DTLSParameters {
        absl::optional<std::vector<Fingerprint>> fingerprints;
        absl::optional<std::string> role;
        FIELDS_MAP("fingerprints", fingerprints, "role", role);
    };

    struct ICECandidate {
        absl::optional<std::string> type;
        absl::optional<std::string> protocol;
        absl::optional<std::string> foundation;
        absl::optional<std::string> ip;
        absl::optional<int32_t> port;
        absl::optional<int32_t> priority;
        FIELDS_MAP("type", type, "protocol", protocol, "foundation", foundation, "ip", ip, "port", port, "priority", priority);
    };

    struct ICEParameters {
        absl::optional<bool> iceLite;
        absl::optional<std::string> password;
        absl::optional<std::string> usernameFragment;
        FIELDS_MAP("iceLite", iceLite, "password", password, "usernameFragment", usernameFragment);
    };

    struct SCTPParameters {
        absl::optional<int32_t> MIS;
        absl::optional<int32_t> OS;
        absl::optional<bool> isDataChannel;
        absl::optional<int32_t> port;
        absl::optional<int32_t> maxMessageSize;
        absl::optional<int32_t> sctpBufferedAmount;
        absl::optional<int32_t> sendBufferSize;
        FIELDS_MAP("MIS", MIS, "OS", OS, "isDataChannel", isDataChannel, "port", port, "maxMessageSize", maxMessageSize, "sctpBufferedAmount", sctpBufferedAmount, "sendBufferSize", sendBufferSize);
    };

    struct Data {
        absl::optional<std::string> id;
        absl::optional<DTLSParameters> dtlsParameters;
        absl::optional<std::vector<ICECandidate>> iceCandidates;
        absl::optional<ICEParameters> iceParameters;
        absl::optional<SCTPParameters> sctpParameters;
        FIELDS_MAP("id", id, "dtlsParameters", dtlsParameters, "iceCandidates", iceCandidates, "iceParameters", iceParameters, "sctpParameters", sctpParameters);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "ok", ok, "data", data);
};

struct JoinRequest {
    struct Device {
        absl::optional<std::string> flag;
        absl::optional<std::string> name;
        absl::optional<std::string> version;
        FIELDS_MAP("flag", flag, "name", name, "version", version);
    };

    struct RTCPFeedback {
        absl::optional<std::string> type;
        absl::optional<std::string> parameter;
        FIELDS_MAP("type", type, "parameter", parameter);
    };

    struct Parameters {
        absl::optional<int32_t> apt;
        absl::optional<int32_t> levelAsymmetryAllowed;
        absl::optional<int32_t> packetizationMode;
        absl::optional<std::string> profileLevelId;
        absl::optional<int32_t> xGoogleStartBitrate;
        absl::optional<int32_t> minptime;
        absl::optional<int32_t> useinbandfec;
        absl::optional<int32_t> profileId;
        FIELDS_MAP("level-asymmetry-allowed", levelAsymmetryAllowed, "packetization-mode", packetizationMode, "profile-level-id", profileLevelId, "x-google-start-bitrate", xGoogleStartBitrate, "minptime", minptime, "useinbandfec", useinbandfec, "profile-id", profileId);
    };

    struct Codec {
        absl::optional<std::string> kind;
        absl::optional<std::string> mimeType;
        absl::optional<int32_t> clockRate;
        absl::optional<int32_t> channels;
        absl::optional<std::vector<RTCPFeedback>> rtcpFeedback;
        absl::optional<Parameters> parameters;
        absl::optional<int32_t> preferredPayloadType;
        FIELDS_MAP("kind", kind, "mimeType", mimeType, "clockRate", clockRate, "channels", channels, "rtcpFeedback", rtcpFeedback, "parameters", parameters, "preferredPayloadType", preferredPayloadType);
    };

    struct HeaderExtension {
        absl::optional<std::string> kind;
        absl::optional<std::string> uri;
        absl::optional<int32_t> preferredId;
        absl::optional<bool> preferredEncrypt;
        absl::optional<std::string> direction;
        FIELDS_MAP("kind", kind, "uri", uri, "preferredId", preferredId, "preferredEncrypt", preferredEncrypt, "direction", direction);
    };

    struct RTPCapabilities {
        absl::optional<std::vector<Codec>> codecs;
        absl::optional<std::vector<HeaderExtension>> headerExtensions;
        FIELDS_MAP("codecs", codecs, "headerExtensions", headerExtensions);
    };

    struct NumStreams {
        absl::optional<int32_t> MIS;
        absl::optional<int32_t> OS;
        FIELDS_MAP("MIS", MIS, "OS", OS);
    };

    struct SCTPCapabilities {
        absl::optional<NumStreams> numStreams;
        FIELDS_MAP("numStreams", numStreams);
    };

    struct Data {
        absl::optional<std::string> displayName;
        absl::optional<Device> device;
        absl::optional<RTPCapabilities> rtpCapabilities;
        absl::optional<SCTPCapabilities> sctpCapabilities;
        FIELDS_MAP("displayName", displayName, "device", device, "rtpCapabilities", rtpCapabilities, "sctpCapabilities", sctpCapabilities);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "join";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct JoinResponse {
    struct Device {
        absl::optional<std::string> flag;
        absl::optional<std::string> name;
        absl::optional<std::string> version;
        FIELDS_MAP("flag", flag, "name", name, "version", version);
    };

    struct Peer {
        absl::optional<std::string> id;
        absl::optional<std::string> displayName;
        absl::optional<Device> device;
        FIELDS_MAP("id", id, "displayName", displayName, "device", device);
    };

    struct Data {
        absl::optional<std::vector<Peer>> peers;
        FIELDS_MAP("peers", peers);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct NewDataConsumerRequest {
    struct AppData {
        absl::optional<std::string> peerId;
        FIELDS_MAP("peerId", peerId);
    };

    struct SCTPStreamParameters {
        absl::optional<int32_t> streamId;
        absl::optional<bool> ordered;
        FIELDS_MAP("streamId", streamId, "ordered", ordered);
    };

    struct Data {
        absl::optional<std::string> id;
        absl::optional<std::string> peerId;
        absl::optional<std::string> dataProducerId;
        absl::optional<std::string> label;
        absl::optional<std::string> protocol;
        absl::optional<SCTPStreamParameters> sctpStreamParameters;
        absl::optional<AppData> appData;
        FIELDS_MAP("id", id, "peerId", peerId, "dataProducerId", dataProducerId, "label", label, "protocol", protocol, "sctpStreamParameters", sctpStreamParameters, "appData", appData);
    };

    absl::optional<bool> request;
    absl::optional<int64_t> id;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct NewConsumerRequest {
    struct AppData {
        absl::optional<std::string> peerId;
        FIELDS_MAP("peerId", peerId);
    };

    struct RTCPFeedback {
        absl::optional<std::string> type;
        absl::optional<std::string> parameter;
        FIELDS_MAP("type", type, "parameter", parameter);
    };

    struct Parameters {
        absl::optional<int32_t> spropstereo;
        absl::optional<int32_t> minptime;
        absl::optional<int32_t> useinbandfec;
        absl::optional<int32_t> usedtx;
        FIELDS_MAP("sprop-stereo", spropstereo, "usedtx", usedtx, "minptime", minptime, "useinbandfec", useinbandfec);
    };

    struct Codec {
        absl::optional<std::string> mimeType;
        absl::optional<int32_t> clockRate;
        absl::optional<int32_t> channels;
        absl::optional<std::vector<RTCPFeedback>> rtcpFeedback;
        absl::optional<Parameters> parameters;
        absl::optional<int32_t> payloadType;
        FIELDS_MAP("mimeType", mimeType, "clockRate", clockRate, "channels", channels, "rtcpFeedback", rtcpFeedback, "parameters", parameters, "payloadType", payloadType);
    };

    struct HeaderExtension {
        absl::optional<int32_t> id;
        absl::optional<bool> encrypt;
        absl::optional<std::string> uri;
        FIELDS_MAP("id", id, "uri", uri, "encrypt", encrypt);
    };

    struct RTX {
        absl::optional<int64_t> ssrc;
        FIELDS_MAP("ssrc", ssrc);
    };

    struct Encoding {
        absl::optional<int32_t> maxBitrate;
        absl::optional<int64_t> ssrc;
        absl::optional<RTX> rtx;
        absl::optional<std::string> scalabilityMode;
        FIELDS_MAP("ssrc", ssrc, "maxBitrate", maxBitrate, "rtx", rtx, "scalabilityMode", scalabilityMode);
    };

    struct Rtcp {
        absl::optional<std::string> cname;
        absl::optional<bool> mux;
        absl::optional<bool> reducedSize;
        FIELDS_MAP("cname", cname, "mux", mux, "reducedSize", reducedSize);
    };

    struct RTPParameters {
        absl::optional<std::vector<Encoding>> encodings;
        absl::optional<std::vector<Codec>> codecs;
        absl::optional<std::vector<HeaderExtension>> headerExtensions;
        absl::optional<std::string> mid;
        absl::optional<Rtcp> rtcp;
        FIELDS_MAP("encodings", encodings, "codecs", codecs, "headerExtensions", headerExtensions, "mid", mid, "rtcp", rtcp);
    };

    struct Data {
        absl::optional<std::string> id;
        absl::optional<std::string> peerId;
        absl::optional<std::string> type;
        absl::optional<std::string> kind;
        absl::optional<std::string> producerId;
        absl::optional<bool> producerPaused;
        absl::optional<RTPParameters> rtpParameters;
        absl::optional<AppData> appData;
        FIELDS_MAP("id", id, "peerId", peerId, "kind", kind, "type", type, "producerId", producerId, "producerPaused", producerPaused, "rtpParameters", rtpParameters, "appData", appData);
    };

    absl::optional<bool> request;
    absl::optional<int64_t> id;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ConnectWebRtcTransportRequest {
    struct Fingerprint {
        absl::optional<std::string> algorithm;
        absl::optional<std::string> value;
        FIELDS_MAP("algorithm", algorithm, "value", value);
    };

    struct DTLSParameters {
        absl::optional<std::vector<Fingerprint>> fingerprints;
        absl::optional<std::string> role;
        FIELDS_MAP("fingerprints", fingerprints, "role", role);
    };

    struct Data {
        absl::optional<std::string> transportId;
        absl::optional<DTLSParameters> dtlsParameters;
        FIELDS_MAP("transportId", transportId, "dtlsParameters", dtlsParameters);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "connectWebRtcTransport";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ProduceRequest {
    struct SharingData {
        // type: screen, windows, camera
        absl::optional<std::string> type;
        FIELDS_MAP("type", type);
    };

    struct AppData {
        absl::optional<std::string> peerId;
        absl::optional<SharingData> sharing;
        FIELDS_MAP("peerId", peerId, "sharing", sharing);
    };

    struct RTCPFeedback {
        absl::optional<std::string> type;
        absl::optional<std::string> parameter;
        FIELDS_MAP("type", type, "parameter", parameter);
    };

    struct Parameters {
        absl::optional<int32_t> apt;
        absl::optional<int32_t> spropstereo;
        absl::optional<int32_t> minptime;
        absl::optional<int32_t> useinbandfec;
        absl::optional<int32_t> usedtx;
        FIELDS_MAP("apt", apt, "sprop-stereo", spropstereo, "usedtx", usedtx, "minptime", minptime, "useinbandfec", useinbandfec);
    };

    struct Codec {
        absl::optional<std::string> mimeType;
        absl::optional<int32_t> clockRate;
        absl::optional<int32_t> channels;
        absl::optional<std::vector<RTCPFeedback>> rtcpFeedback;
        absl::optional<Parameters> parameters;
        absl::optional<int32_t> payloadType;
        FIELDS_MAP("mimeType", mimeType, "clockRate", clockRate, "channels", channels, "rtcpFeedback", rtcpFeedback, "parameters", parameters, "payloadType", payloadType);
    };

    struct HeaderExtension {
        absl::optional<int32_t> id;
        absl::optional<bool> encrypt;
        absl::optional<std::string> uri;
        FIELDS_MAP("id", id, "uri", uri, "encrypt", encrypt);
    };

    struct Encoding {
        absl::optional<int64_t> ssrc;
        absl::optional<bool> active;
        absl::optional<bool> dtx;
        absl::optional<int32_t> maxBitrate;
        absl::optional<std::string> rid;
        absl::optional<std::string> scalabilityMode;
        absl::optional<float> scaleResolutionDownBy;
        FIELDS_MAP("ssrc", ssrc, "active", active, "dtx", dtx, "maxBitrate", maxBitrate, "rid", rid, "scalabilityMode", scalabilityMode, "scaleResolutionDownBy", scaleResolutionDownBy);
    };

    struct Rtcp {
        absl::optional<std::string> cname;
        absl::optional<bool> reducedSize;
        FIELDS_MAP("cname", cname, "reducedSize", reducedSize);
    };

    struct RTPParameters {
        absl::optional<std::vector<Encoding>> encodings;
        absl::optional<std::vector<Codec>> codecs;
        absl::optional<std::vector<HeaderExtension>> headerExtensions;
        absl::optional<std::string> mid;
        absl::optional<Rtcp> rtcp;
        FIELDS_MAP("encodings", encodings, "codecs", codecs, "headerExtensions", headerExtensions, "mid", mid, "rtcp", rtcp);
    };

    struct Data {
        absl::optional<std::string> transportId;
        absl::optional<std::string> kind;
        absl::optional<RTPParameters> rtpParameters;
        absl::optional<AppData> appData;
        FIELDS_MAP("transportId", transportId, "kind", kind, "rtpParameters", rtpParameters, "appData", appData);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "produce";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ProduceResponse {
    struct Data {
        absl::optional<std::string> id;
        FIELDS_MAP("id", id);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct ProduceDataRequest {
    struct AppData {
        absl::optional<std::string> info;
        FIELDS_MAP("info", info);
    };

    struct SCTPStreamParameters {
        absl::optional<int32_t> streamId;
        absl::optional<int32_t> maxRetransmits;
        absl::optional<bool> ordered;
        FIELDS_MAP("streamId", streamId, "maxRetransmits", maxRetransmits, "ordered", ordered);
    };

    struct Data {
        absl::optional<std::string> transportId;
        absl::optional<std::string> label;
        absl::optional<std::string> protocol;
        absl::optional<SCTPStreamParameters> sctpStreamParameters;
        absl::optional<AppData> appData;
        FIELDS_MAP("transportId", transportId, "label", label, "protocol", protocol, "sctpStreamParameters", sctpStreamParameters, "appData", appData);
    };

    absl::optional<bool> request;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "produceData";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ProduceDataResponse {
    struct Data {
        absl::optional<std::string> id;
        FIELDS_MAP("id", id);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct ProducerRequest {
    struct Data {
        absl::optional<std::string> producerId;
        FIELDS_MAP("producerId", producerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    // method: 'pauseProducer', 'resumeProducer', 'closeProducer'
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ConsumerRequest {
    struct Data {
        absl::optional<std::string> consumerId;
        FIELDS_MAP("consumerId", consumerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    // method: 'pauseConsumer', 'resumeConsumer'
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ChangeDisplayNameRequest {
    struct Data {
        absl::optional<std::string> displayName;
        FIELDS_MAP("displayName", displayName);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "changeDisplayName";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct RestartICERequest {
    struct Data {
        absl::optional<std::string> transportId;
        FIELDS_MAP("transportId", transportId);
    };

    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();
    absl::optional<std::string> method = "restartIce";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct RestartICEResponse {
    struct Data {
        absl::optional<bool> iceLite;
        absl::optional<std::string> password;
        absl::optional<std::string> usernameFragment;
        FIELDS_MAP("iceLite", iceLite, "password", password, "usernameFragment", usernameFragment);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<Data> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct SetConsumerPriorityRequest {
    struct Data {
        absl::optional<std::string> consumerId;
        absl::optional<int32_t> priority;
        FIELDS_MAP("consumerId", consumerId, "priority", priority);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "setConsumerPriority";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct SetConsumerPreferredLayersRequest {
    struct Data {
        absl::optional<std::string> consumerId;
        absl::optional<int32_t> spatialLayer;
        absl::optional<int32_t> temporalLayer;
        FIELDS_MAP("consumerId", consumerId, "spatialLayer", spatialLayer, "temporalLayer", temporalLayer);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "setConsumerPreferredLayers";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetTransportStatsRequest {
    struct Data {
        absl::optional<std::string> transportId;
        FIELDS_MAP("transportId", transportId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "getTransportStats";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetTransportStatsResponse {
    struct ICESelectedTuple {
        absl::optional<std::string> localIp;
        absl::optional<int32_t> localPort;
        absl::optional<std::string> remoteIp;
        absl::optional<int32_t> remotePort;
        absl::optional<std::string> protocol;
        FIELDS_MAP("localIp", localIp, "localPort", localPort, "remoteIp", remoteIp, "remotePort", remotePort, "protocol", protocol);
    };

    struct Data {
        absl::optional<std::string> transportId;
        absl::optional<std::string> type;
        absl::optional<int64_t> availableOutgoingBitrate;
        absl::optional<int64_t> bytesReceived;
        absl::optional<int64_t> bytesSent;
        absl::optional<std::string> dtlsState;
        absl::optional<std::string> iceRole;
        absl::optional<ICESelectedTuple> iceSelectedTuple;
        absl::optional<std::string> iceState;
        absl::optional<int64_t> maxIncomingBitrate;
        absl::optional<int64_t> probationBytesSent;
        absl::optional<int64_t> probationSendBitrate;
        absl::optional<int64_t> recvBitrate;
        absl::optional<int64_t> rtpBytesReceived;
        absl::optional<int64_t> rtpBytesSent;
        absl::optional<int64_t> rtpPacketLossReceived;
        absl::optional<int64_t> rtpRecvBitrate;
        absl::optional<int64_t> rtpSendBitrate;
        absl::optional<int64_t> rtxBytesReceived;
        absl::optional<int64_t> rtxBytesSent;
        absl::optional<int64_t> rtxRecvBitrate;
        absl::optional<int64_t> rtxSendBitrate;
        absl::optional<std::string> sctpState;
        absl::optional<int64_t> sendBitrate;
        absl::optional<int64_t> timestamp;
        FIELDS_MAP("transportId", transportId,
                   "type", type,
                   "availableOutgoingBitrate", availableOutgoingBitrate,
                   "bytesReceived", bytesReceived,
                   "bytesSent", bytesSent,
                   "dtlsState", dtlsState,
                   "iceRole", iceRole,
                   "iceSelectedTuple", iceSelectedTuple,
                   "iceState", iceState,
                   "maxIncomingBitrate", maxIncomingBitrate,
                   "probationBytesSent", probationBytesSent,
                   "probationSendBitrate", probationSendBitrate,
                   "recvBitrate", recvBitrate,
                   "rtpBytesReceived", rtpBytesReceived,
                   "rtpBytesSent", rtpBytesSent,
                   "rtpPacketLossReceived", rtpPacketLossReceived,
                   "rtpSendBitrate", rtpSendBitrate,
                   "rtxBytesReceived", rtxBytesReceived,
                   "rtxBytesSent", rtxBytesSent,
                   "rtxRecvBitrate", rtxRecvBitrate,
                   "rtxSendBitrate", rtxSendBitrate,
                   "sctpState", sctpState,
                   "sendBitrate", sendBitrate,
                   "timestamp", timestamp);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<std::vector<Data>> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct GetProducerStatsRequest {
    struct Data {
        absl::optional<std::string> producerId;
        FIELDS_MAP("producerId", producerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "getProducerStats";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetProducerStatsResponse {
    struct BitrateByLayer {
        absl::optional<int32_t> L00;
        absl::optional<int32_t> L01;
        absl::optional<int32_t> L02;
        FIELDS_MAP("0.0", L00, "0.1", L01, "0.2", L02);
    };

    struct Data {
        absl::optional<std::string> type;
        absl::optional<std::string> kind;
        absl::optional<std::string> mimeType;
        absl::optional<std::string> rid;
        absl::optional<int64_t> bitrate;
        absl::optional<int64_t> byteCount;
        absl::optional<int64_t> firCount;
        absl::optional<int64_t> fractionLost;
        absl::optional<int64_t> jitter;
        absl::optional<int64_t> nackCount;
        absl::optional<int64_t> nackPacketCount;
        absl::optional<int64_t> packetCount;
        absl::optional<int64_t> packetsDiscarded;
        absl::optional<int64_t> packetsLost;
        absl::optional<int64_t> packetsRepaired;
        absl::optional<int64_t> packetsRetransmitted;
        absl::optional<int64_t> pliCount;
        absl::optional<double> roundTripTime;
        absl::optional<int64_t> rtxPacketsDiscarded;
        absl::optional<int64_t> rtxSsrc;
        absl::optional<int64_t> score;
        absl::optional<int64_t> ssrc;
        absl::optional<int64_t> timestamp;
        absl::optional<BitrateByLayer> bitrateByLayer;
        FIELDS_MAP("type", type,
                   "kind", kind,
                   "mimeType", mimeType,
                   "rid", rid,
                   "bitrate", bitrate,
                   "byteCount", byteCount,
                   "firCount", firCount,
                   "fractionLost", fractionLost,
                   "jitter", jitter,
                   "nackCount", nackCount,
                   "nackPacketCount", nackPacketCount,
                   "packetCount", packetCount,
                   "packetsDiscarded", packetsDiscarded,
                   "packetsLost", packetsLost,
                   "packetsRepaired", packetsRepaired,
                   "packetsRetransmitted", packetsRetransmitted,
                   "pliCount", pliCount,
                   "roundTripTime", roundTripTime,
                   "rtxPacketsDiscarded", rtxPacketsDiscarded,
                   "rtxSsrc", rtxSsrc,
                   "score", score,
                   "ssrc", ssrc,
                   "timestamp", timestamp,
                   "bitrateByLayer", bitrateByLayer);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<std::vector<Data>> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct GetDataProducerStatsRequest {
    struct Data {
        absl::optional<std::string> dataProducerId;
        FIELDS_MAP("dataProducerId", dataProducerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "getDataProducerStats";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetDataProducerStatsResponse {
    struct Data {
        absl::optional<std::string> type;
        absl::optional<std::string> label;
        absl::optional<std::string> protocol;
        absl::optional<int64_t> bytesReceived;
        absl::optional<int64_t> messagesReceived;
        absl::optional<int64_t> timestamp;
        FIELDS_MAP("type", type,
                   "label", label,
                   "protocol", protocol,
                   "bytesReceived", bytesReceived,
                   "messagesReceived", messagesReceived,
                   "timestamp", timestamp);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<std::vector<Data>> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct GetConsumerStatsRequest {
    struct Data {
        absl::optional<std::string> consumerId;
        FIELDS_MAP("consumerId", consumerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "getConsumerStats";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetConsumerStatsResponse {
    struct BitrateByLayer {
        absl::optional<int32_t> L00;
        absl::optional<int32_t> L01;
        absl::optional<int32_t> L02;
        FIELDS_MAP("0.0", L00, "0.1", L01, "0.2", L02);
    };
    struct Data {
        absl::optional<std::string> type;
        absl::optional<std::string> kind;
        absl::optional<std::string> mimeType;
        absl::optional<std::string> rid;
        absl::optional<int64_t> bitrate;
        absl::optional<int64_t> byteCount;
        absl::optional<int64_t> firCount;
        absl::optional<int64_t> fractionLost;
        absl::optional<int64_t> jitter;
        absl::optional<int64_t> nackCount;
        absl::optional<int64_t> nackPacketCount;
        absl::optional<int64_t> packetCount;
        absl::optional<int64_t> packetsDiscarded;
        absl::optional<int64_t> packetsLost;
        absl::optional<int64_t> packetsRepaired;
        absl::optional<int64_t> packetsRetransmitted;
        absl::optional<int64_t> pliCount;
        absl::optional<int64_t> rtxPacketsDiscarded;
        absl::optional<int64_t> rtxSsrc;
        absl::optional<double> roundTripTime;
        absl::optional<int64_t> score;
        absl::optional<int64_t> ssrc;
        absl::optional<int64_t> timestamp;
        absl::optional<BitrateByLayer> bitrateByLayer;
        FIELDS_MAP("type", type,
                   "kind", kind,
                   "mimeType", mimeType,
                   "rid", rid,
                   "bitrate", bitrate,
                   "byteCount", byteCount,
                   "firCount", firCount,
                   "fractionLost", fractionLost,
                   "jitter", jitter,
                   "nackCount", nackCount,
                   "nackPacketCount", nackPacketCount,
                   "packetCount", packetCount,
                   "packetsDiscarded", packetsDiscarded,
                   "packetsLost", packetsLost,
                   "packetsRepaired", packetsRepaired,
                   "packetsRetransmitted", packetsRetransmitted,
                   "pliCount", pliCount,
                   "rtxPacketsDiscarded", rtxPacketsDiscarded,
                   "rtxSsrc", rtxSsrc,
                   "roundTripTime", roundTripTime,
                   "score", score,
                   "ssrc", ssrc,
                   "timestamp", timestamp,
                   "bitrateByLayer", bitrateByLayer);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<std::vector<Data>> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct GetDataConsumerStatsRequest {
    struct Data {
        absl::optional<std::string> dataConsumerId;
        FIELDS_MAP("dataConsumerId", dataConsumerId);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "getDataConsumerStats";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct GetDataConsumerStatsResponse {
    struct Data {
        absl::optional<std::string> type;
        absl::optional<std::string> label;
        absl::optional<std::string> protocol;
        absl::optional<int64_t> bufferedAmount;
        absl::optional<int64_t> bytesSent;
        absl::optional<int64_t> messagesSent;
        absl::optional<int64_t> timestamp;
        FIELDS_MAP("type", type,
                   "label", label,
                   "protocol", protocol,
                   "bufferedAmount", bufferedAmount,
                   "bytesSent", bytesSent,
                   "messagesSent", messagesSent,
                   "timestamp", timestamp);
    };

    absl::optional<bool> response;
    absl::optional<int64_t> id;
    absl::optional<bool> ok;
    absl::optional<std::vector<Data>> data;
    FIELDS_MAP("response", response, "id", id, "ok", ok, "data", data);
};

struct ApplyNetworkThrottleRequest {
    struct Data {
        absl::optional<std::string> secret;
        absl::optional<int64_t> uplink;
        absl::optional<int64_t> downlink;
        absl::optional<int64_t> rtt;
        FIELDS_MAP("secret", secret, "uplink", uplink, "downlink", downlink, "rtt", rtt);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "applyNetworkThrottle";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct ResetNetworkThrottleRequest {
    struct Data {
        absl::optional<std::string> secret;
        FIELDS_MAP("secret", secret);
    };
    absl::optional<bool> request = true;
    absl::optional<int64_t> id = generateID();

    absl::optional<std::string> method = "resetNetworkThrottle";
    absl::optional<Data> data;
    FIELDS_MAP("request", request, "id", id, "method", method, "data", data);
};

struct DownlinkBweNotification {
    struct Data {
        absl::optional<int64_t> availableBitrate;
        absl::optional<int64_t> desiredBitrate;
        absl::optional<int64_t> effectiveDesiredBitrate;
        FIELDS_MAP("availableBitrate", availableBitrate,
                   "desiredBitrate", desiredBitrate,
                   "effectiveDesiredBitrate", effectiveDesiredBitrate);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ConsumerScoreNotification {
    struct Score {
        absl::optional<int32_t> producerScore;
        absl::optional<std::vector<int32_t>> producerScores;
        absl::optional<int32_t> score;
        FIELDS_MAP("producerScore", producerScore, "producerScores", producerScores, "score", score);
    };

    struct Data {
        absl::optional<std::string> consumerId;
        absl::optional<Score> score;
        FIELDS_MAP("consumerId", consumerId, "score", score);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ProducerScoreNotification {
    struct Score {
        absl::optional<std::string> rid;
        absl::optional<int32_t> encodingIdx;
        absl::optional<int32_t> score;
        absl::optional<int64_t> ssrc;
        FIELDS_MAP("rid", rid, "encodingIdx", encodingIdx, "score", score, "ssrc", ssrc);
    };

    struct Data {
        absl::optional<std::string> producerId;
        absl::optional<std::vector<Score>> score;
        FIELDS_MAP("producerId", producerId, "score", score);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ConsumerLayersChangedNotification {
    struct Data {
        absl::optional<std::string> consumerId;
        absl::optional<int32_t> spatialLayer;
        absl::optional<int32_t> temporalLayer;
        FIELDS_MAP("consumerId", consumerId, "spatialLayer", spatialLayer, "temporalLayer", temporalLayer);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ActiveSpeakerNotification {
    struct Data {
        absl::optional<std::string> peerId;
        absl::optional<int32_t> volume;
        FIELDS_MAP("peerId", peerId, "volume", volume);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ConsumerPausedNotification {
    struct Data {
        absl::optional<std::string> consumerId;
        FIELDS_MAP("consumerId", consumerId);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ConsumerResumedNotification {
    struct Data {
        absl::optional<std::string> consumerId;
        FIELDS_MAP("consumerId", consumerId);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct ConsumerClosedNotification {
    struct Data {
        absl::optional<std::string> consumerId;
        FIELDS_MAP("consumerId", consumerId);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct DataConsumerClosedNotification {
    struct Data {
        absl::optional<std::string> dataConsumerId;
        FIELDS_MAP("dataConsumerId", dataConsumerId);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct PeerDisplayNameChangedNotification {
    struct Data {
        absl::optional<std::string> peerId;
        absl::optional<std::string> oldDisplayName;
        absl::optional<std::string> displayName;
        FIELDS_MAP("peerId", peerId, "oldDisplayName", oldDisplayName, "displayName", displayName);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct NewPeerNotification {
    struct Device {
        absl::optional<std::string> flag;
        absl::optional<std::string> name;
        absl::optional<std::string> version;
        FIELDS_MAP("flag", flag, "name", name, "version", version);
    };

    struct Data {
        absl::optional<std::string> id;
        absl::optional<std::string> displayName;
        absl::optional<Device> device;
        FIELDS_MAP("id", id, "displayName", displayName, "device", device);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct PeerClosedNotification {
    struct Data {
        absl::optional<std::string> peerId;
        FIELDS_MAP("peerId", peerId);
    };

    absl::optional<bool> notification;
    absl::optional<std::string> method;
    absl::optional<Data> data;
    FIELDS_MAP("notification", notification, "method", method, "data", data);
};

struct Sharing {
    // type: screen, windows, camera
    std::string type;
};

void to_json(nlohmann::json& j, const Sharing& st);

void from_json(const nlohmann::json& j, Sharing& st);

struct SharingAppData {
    Sharing sharing;
};

void to_json(nlohmann::json& j, const SharingAppData& st);

void from_json(const nlohmann::json& j, SharingAppData& st);

}
