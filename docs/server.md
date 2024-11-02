## Server

This code represents a simple WebSocket server using **Socket.IO** to manage real-time communication between clients. We implemented this server using JavaScript.

### Main Challenges

The main challenge in our project was that the ID users want to use to call each other and send SDPs must be unique, and the server must also be aware of it. So, at the beginning of the client's connection with the server, we send a socket ID assigned to the client itself, ensuring it is unique on the server and that the server is aware of it (`io.on('connection', (socket) => { ... })`). In every SDP offer and answer, the client includes this ID for the target client it wants to send the SDP to, so the server can find and send the SDP to the correct recipient. Additionally, along with the SDP, the server sends the sender's ID to the target client, allowing it to identify who sent the SDP.

### **Server Initialization**

- **Initialization**: The server is initialized to run on port 3000 with `const io = require('socket.io')(3000);`.
- **Client Management**: The `clients` object stores connected clients using their unique `socket.id` as the key.

### **Connection Handling**

- **Connection Event**: The `io.on('connection', (socket) => { ... })` block manages new client connections.
  - When a new client connects, their `socket.id` is logged and stored in the `clients` object.
  - The server emits the `your_id` event to inform the client their assigned ID.

### **Handling SDP Messages**

- **Purpose**: Handles both "offer" and "answer" SDP messages.
- **Methods**:
  - `handle_sdp_messages(socket, data, type)`: Handles incoming SDP messages (either "offer" or "answer").
    - **Parsing**: The data is parsed, and the target client ID and SDP content are extracted.
    - **Relay**: If the target client exists, the server emits the SDP message (`type + '_sdp'`) to that client.

### **Handling ICE Candidates**

- **Purpose**: ICE candidates are exchanged to facilitate WebRTC's connection traversal.
- **Method**:
  - `handle_ice_messages(socket, data)`: Processes incoming ICE candidate messages.
    - **Parsing**: Extracts the target client ID, candidate details, and media identifier (MID).
    - **Relay**: If the target client is connected, the server forwards the ICE candidate via the `send_ice` event.

### Methods

- **`io.on('connection', ...)`**: Handles new client connections and assigns a unique `socket.id`.
- **`handle_sdp_messages()`**: Processes and forwards SDP "offer" and "answer" messages to target clients.
- **`handle_ice_messages()`**: Processes and forwards ICE candidate information to target clients.
- **`socket.on('disconnect', ...)`**: Manages client disconnections and removes them from the `clients` object.

### Events

- **`offer_sdp`**: Event received when a client sends an SDP "offer".
- **`answer_sdp`**: Event received for SDP "answer".
- **`send_ice`**: Event for relaying ICE candidates.
- **`your_id`**: Notifies the client of its unique socket ID.
