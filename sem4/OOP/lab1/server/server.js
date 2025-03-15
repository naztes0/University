import 'dotenv/config'
import express from 'express'
import cors from 'cors'

//App config

const PORT=process.env.PORT||4000;
const app =express();

//Initialize Middlewares
app.use(express.json())
app.use(cors())

//API routes
app.get('/',(req,res)=>res.send("API Working"))
app.listen(PORT,()=>console.log("Server Running on port "+PORT))
