import 'dotenv/config'
import express from 'express'
import cors from 'cors'
import connectDB from './configs/mongodb.js'
import userRouter from './routes/userRoutes.js';
import imageRouter from './routes/imageRoutes.js';
import config from './configs/appConfig.js';

//App config

const PORT = config.server.port;
const app = express();


//Initialize Middlewares
app.use(express.json())
app.use(cors())

//API routes
app.get('/', (req, res) => res.send("API Working"))
app.use('/api/user', userRouter)
app.use('/api/image', imageRouter)

app.listen(PORT, () => console.log("Server Running on port " + PORT))

connectDB()
  .then(() => {
  })
  .catch(err => {
    console.error("DB Connection Error:", err);
  });
