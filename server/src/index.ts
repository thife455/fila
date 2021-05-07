import express from "express";
import { Server } from "socket.io";
import connectionHandler from "../entities/connectionHandler";
import session from "express-session";
import redis from "redis";

let RedisStore = require("connect-redis")(session);

const app = express();
const http = require("http");
const server = http.createServer(app);
const io = new Server(server);
const handler = new connectionHandler();
let redisClient = redis.createClient();

// Change secret to variable
app.use(
  session({
    secret: "keyboard cat",
    resave: false,
    saveUninitialized: true,
    store: new RedisStore({ client: redisClient }),
    cookie: { maxAge: 6000000 },
  })
);

let index: number = 0;
app.get("/", function (req, res) {
  res.sendFile(__dirname + "/index.html");
  if (req.session.index) {
  } else {
    req.session.index = handler.clientOrder.length + 1;
    handler.createClient(req.session.index);
  }
  index = req.session.index;
  console.log(req.session.index);
  
  // console.log(handler.clientOrder)
});

io.on("connection", (socket) => {
  console.log("user connected");
  socket.emit("hello", index);
  socket.on("disconnect", () => {
    console.log("user disconnected");
  });
});

server.listen(3000, () => {
  console.log("listening on *:3000");
});
