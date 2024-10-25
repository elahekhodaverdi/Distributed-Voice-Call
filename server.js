const io = require('socket.io')(3000);

const clients = {};

// ANSI escape codes for color
const COLORS = {
    reset: "\x1b[0m",
    blue: "\x1b[34m",
    green: "\x1b[32m",
    cyan: "\x1b[36m",
    magenta: "\x1b[35m",
    yellow: "\x1b[33m",
    red: "\x1b[31m",
};

function colorText(text, color) {
    return `${color}${text}${COLORS.reset}`;
}

function truncateString(str, maxLength = 100) {
    return str.length > maxLength ? str.substring(0, maxLength) + '...' : str;
}

function handle_sdp_messages(socket, data, type) {
    console.log(colorText(`Received ${type} SDP message`, COLORS.blue));
    data = JSON.parse(data);
    const targetClientId = data.targetClientId;
    const sdp = data.sdp;

    stringSdp = JSON.parse(sdp);
    console.log(colorText(`Target Client ID: ${truncateString(targetClientId)}`, COLORS.green));
    console.log(colorText(`SDP type: ${truncateString(stringSdp.type)}`, COLORS.cyan));
    console.log(colorText(`SDP sdp: ${truncateString(stringSdp.sdp)}`, COLORS.cyan));

    if (clients[targetClientId]) {
        clients[targetClientId].emit(type + '_sdp', {
            from: socket.id,
            sdp: sdp
        });
    } else {
        socket.emit('error', { message: 'Target client not connected' });
        console.log(colorText(`Error: Target client ${targetClientId} not connected`, COLORS.red));
    }
}

function handle_ice_messages(socket, data) {
    console.log(colorText("Received ICE candidate message", COLORS.blue));
    data = JSON.parse(data);
    const targetClientId = data.targetClientId;
    const candidate = data.candidate;
    const mid = data.mid;

    console.log(colorText(`Target Client ID: ${truncateString(targetClientId)}`, COLORS.green));
    console.log(colorText(`ICE Candidate: ${truncateString(candidate)}`, COLORS.magenta));
    console.log(colorText(`MID: ${truncateString(mid)}`, COLORS.yellow));

    if (clients[targetClientId]) {
        clients[targetClientId].emit('send_ice', {
            from: socket.id,
            candidate: candidate,
            mid: mid
        });
    } else {
        socket.emit('error', { message: 'Target client not connected' });
        console.log(colorText(`Error: Target client ${targetClientId} not connected`, COLORS.red));
    }
}

io.on('connection', (socket) => {
    console.log(colorText('New client connected:', COLORS.green), colorText(socket.id, COLORS.yellow));

    clients[socket.id] = socket;

    socket.emit('your_id', socket.id);

    socket.on('offer_sdp', (data) => handle_sdp_messages(socket, data, "offer"));

    socket.on('answer_sdp', (data) => handle_sdp_messages(socket, data, "answer"));

    socket.on("send_ice", (data) => handle_ice_messages(socket, data));

    socket.on('disconnect', () => {
        console.log(colorText('Client disconnected:', COLORS.red), colorText(socket.id, COLORS.yellow));
        delete clients[socket.id];
    });
});
