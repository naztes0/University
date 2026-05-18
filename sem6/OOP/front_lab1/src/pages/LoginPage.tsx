import { useState } from 'react'
import apiClient from '../api/client'

interface Props {
    onLogin: (token: string, role: string) => void
}

function LoginPage({ onLogin }: Props) {
    const [isRegister, setIsRegister] = useState(false)
    const [name, setName] = useState('')
    const [email, setEmail] = useState('')
    const [password, setPassword] = useState('')
    const [error, setError] = useState<string | null>(null)
    const [loading, setLoading] = useState(false)

    const handleSubmit = async () => {
        setError(null)
        setLoading(true)
        try {
            const endpoint = isRegister ? '/auth/register' : '/auth/login'
            const body = isRegister ? { name, email, password } : { email, password }
            const response = await apiClient.post(endpoint, body)
            const { token, role } = response.data
            localStorage.setItem('auth_token', token)
            localStorage.setItem('auth_role', role)
            onLogin(token, role)
        } catch (err: any) {
            setError(err.response?.data?.error || 'Something went wrong')
        } finally {
            setLoading(false)
        }
    }

    return (
        <div style={{
            minHeight: '100vh',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            background: 'linear-gradient(135deg, #e7f5ff 0%, #f8f9fa 100%)',
        }}>
            <div style={{
                background: 'white',
                borderRadius: '16px',
                padding: '40px',
                width: '380px',
                boxShadow: '0 4px 24px rgba(0,0,0,0.08)',
            }}>
                <div style={{ textAlign: 'center', marginBottom: '32px' }}>
                    <div style={{ fontSize: '40px', marginBottom: '8px' }}>🍽️</div>
                    <h1 style={{ fontSize: '24px', fontWeight: 700, color: '#212529' }}>Restaurant App</h1>
                    <p style={{ color: '#868e96', marginTop: '4px', fontSize: '14px' }}>
                        {isRegister ? 'Create your account' : 'Welcome back'}
                    </p>
                </div>

                <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
                    {isRegister && (
                        <input
                            placeholder="Full name"
                            value={name}
                            onChange={e => setName(e.target.value)}
                        />
                    )}
                    <input
                        placeholder="Email address"
                        type="email"
                        value={email}
                        onChange={e => setEmail(e.target.value)}
                    />
                    <input
                        placeholder="Password"
                        type="password"
                        value={password}
                        onChange={e => setPassword(e.target.value)}
                        onKeyDown={e => e.key === 'Enter' && handleSubmit()}
                    />

                    {error && (
                        <div style={{
                            background: '#fff5f5',
                            border: '1px solid #ffc9c9',
                            borderRadius: '8px',
                            padding: '10px 14px',
                            color: '#c92a2a',
                            fontSize: '14px',
                        }}>
                            {error}
                        </div>
                    )}

                    <button
                        onClick={handleSubmit}
                        disabled={loading}
                        style={{
                            background: '#4dabf7',
                            color: 'white',
                            padding: '12px',
                            fontSize: '15px',
                            fontWeight: 600,
                            marginTop: '4px',
                        }}
                    >
                        {loading ? 'Loading...' : isRegister ? 'Create Account' : 'Sign In'}
                    </button>

                    <button
                        onClick={() => { setIsRegister(!isRegister); setError(null) }}
                        style={{
                            background: 'none',
                            color: '#4dabf7',
                            padding: '8px',
                            fontSize: '14px',
                            textDecoration: 'underline',
                        }}
                    >
                        {isRegister ? 'Already have an account? Sign in' : "Don't have an account? Register"}
                    </button>
                </div>
            </div>
        </div>
    )
}

export default LoginPage