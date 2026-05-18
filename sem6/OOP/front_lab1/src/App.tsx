import { useState, useEffect } from 'react'
import { Routes, Route, Navigate } from 'react-router-dom'
import MenuPage from './pages/MenuPage'
import OrdersPage from './pages/OrdersPage'
import AdminPage from './pages/AdminPage'
import KitchenPage from './pages/KitchenPage'
import LoginPage from './pages/LoginPage'
import Navbar from './components/Navbar'

function App() {
  const [token, setToken] = useState<string | null>(localStorage.getItem('auth_token'))
  const [role, setRole] = useState<string | null>(localStorage.getItem('auth_role'))

  useEffect(() => {
    const handleStorage = () => {
      setToken(localStorage.getItem('auth_token'))
      setRole(localStorage.getItem('auth_role'))
    }
    window.addEventListener('storage', handleStorage)
    return () => window.removeEventListener('storage', handleStorage)
  }, [])

  if (!token) {
    return <LoginPage onLogin={(t, r) => { setToken(t); setRole(r) }} />
  }

  return (
    <div>
      <Navbar onLogout={() => { setToken(null); setRole(null) }} role={role} />
      <Routes>
        <Route path="/" element={<Navigate to="/menu" />} />
        <Route path="/menu" element={<MenuPage />} />
        <Route path="/orders" element={<OrdersPage />} />
        {role === 'ADMIN' && <Route path="/admin" element={<AdminPage />} />}
        {role === 'KITCHEN' && <Route path="/kitchen" element={<KitchenPage />} />}
        <Route path="*" element={<Navigate to="/menu" />} />
      </Routes>
    </div>
  )
}

export default App