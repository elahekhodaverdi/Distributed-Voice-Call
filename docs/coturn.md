## WebRTC

WebRTC (Web Real-Time Communication) is a technology that enables real-time audio, video, and data sharing between browsers and devices, directly in a peer-to-peer (P2P) manner without needing plugins or additional software.

## Components of WebRTC

### STUN Server (Session Traversal Utilities for NAT)

* **What it does**:
A STUN server helps devices identify their public IP address. This is important because most devices are behind NAT (Network Address Translation), which hides a device's internal IP address on a private network. When two users want to connect via WebRTC, each needs to know its public-facing IP address to connect directly (peer-to-peer).

* **Advantages**:

    * Low Resource Demand: STUN servers don’t require much processing power or bandwidth because they don’t handle the media/data traffic directly.
    * Direct Connection: They help establish a direct connection, 
    which keeps latency low and minimizes server dependency.

* **Disadvantages**:

    * Limited to Simple NATs: STUN works well only for simpler NAT types. For complex NAT configurations or firewalls, STUN alone may not be sufficient to establish a connection.
    * Relies on Open Ports: STUN requires certain ports to be open, and strict network firewalls can block these, making STUN ineffective in some cases.

### TURN Server (Traversal Using Relays around NAT)
* **What it does**:
A TURN server acts as a relay between two devices when a direct connection isn’t possible. If NAT or firewalls block the peer-to-peer connection, both devices send their data to the TURN server, which then relays it between them. This makes TURN essential for ensuring connectivity in cases where STUN alone can’t establish a connection.

* **Advantages**:

    * Guaranteed Connectivity: TURN servers can work even when there are complex NATs or firewalls that would otherwise block a direct peer-to-peer connection.
    * Reliable Fallback: They provide a fallback for situations where direct connections are not possible, increasing WebRTC’s reliability.
* **Disadvantages**:

    *Increased Latency: Since data is relayed through the TURN server rather than going directly between peers, it takes a longer path, which increases latency.
    * Higher Costs: TURN servers consume more resources because they handle the actual media traffic. They require higher bandwidth and server capacity, which can be expensive for large-scale applications.

**What is Coturn?** Coturn is an open-source TURN and STUN server that provides both functionalities in a single package. It’s widely used in WebRTC applications to ensure connectivity between peers, even in challenging network environments.
    
### Signaling Server
* **What it does**:
The signaling server is responsible for exchanging connection information between peers to set up the WebRTC connection. This involves sharing the necessary metadata (such as IP addresses and network ports) and negotiating connection parameters. Importantly, the signaling server is only involved during the setup phase, not during the actual data transmission.

* **Advantages**:

    * Flexibility in Communication: The signaling server doesn’t dictate the communication protocol, meaning developers can use various protocols (like WebSocket, HTTP) based on their needs.
    * Security and Session Management: It can handle user authentication, session management, and secure key exchanges, making the initial connection setup more secure.
* **Disadvantages**:

    * Additional Component: A signaling server is an extra piece of infrastructure to maintain, adding to the complexity of a WebRTC system.
    * Potential Point of Failure: Since all initial connections depend on it, if the signaling server goes down, new connections can’t be established until it’s restored.

### What did we do in this project?
In this project, we managed to get a free VPS from Azure using a student subscription (which took quite a bit of effort!). We set up Coturn on the VPS to run our own STUN and TURN server, enabling seamless peer-to-peer connectivity for our application. Additionally, we deployed our signaling server on the same VPS, so everything runs online and users can connect without any manual setup or initialization, making our program ready to go right out of the box.










