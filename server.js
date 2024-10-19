const io = require('socket.io')(3000);

const clients = {};

io.on('connection', (socket) => {
    console.log('New client connected:', socket.id);

    clients[socket.id] = socket;

    socket.emit('your_id', socket.id);

    socket.on('send_message', (data) => {
        const targetClientId = data.targetClientId;
        const message = data.message;

        if (clients[targetClientId]) {
            clients[targetClientId].emit('receive_message', {
                from: socket.id,
                message: message
            });
        } else {
            socket.emit('error', { message: 'Target client not connected' });
        }
    });

    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
        delete clients[socket.id];
    });
});
