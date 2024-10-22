const io = require('socket.io')(3000);

const clients = {};

function handle_sdp_messages(socket, data, type){
    data = JSON.parse(data);
    console.log("new offer sdp msg")
    const targetClientId = data.targetClientId;
    const sdp = data.sdp;
    if (clients[targetClientId]) {
        clients[targetClientId].emit(type + '_sdp', {
            from: socket.id,
            sdp: sdp
        });
    } else {
        socket.emit('error', { message: 'Target client not connected' });
    }
}

io.on('connection', (socket) => {
    console.log('New client connected:', socket.id);

    clients[socket.id] = socket;

    socket.emit('your_id', socket.id);

    socket.on('offer_sdp', (data) => handle_sdp_messages(socket, data, "offer"));

    socket.on('answer_sdp', (data) => handle_sdp_messages(socket, data, "answer"));

    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
        delete clients[socket.id];
    });

    
});
