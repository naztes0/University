import { Link, useLocation } from 'react-router-dom'

interface Props {
    onLogout: () => void
    role: string | null
}

function Navbar({ onLogout, role }: Props) {
    const location = useLocation()

    const handleLogout = () => {
        localStorage.removeItem('auth_token')
        localStorage.removeItem('auth_role')
        onLogout()
    }

    const linkStyle = (path: string) => ({
        color: location.pathname === path ? '#4dabf7' : '#495057',
        textDecoration: 'none',
        fontWeight: location.pathname === path ? 600 : 400,
        fontSize: '15px',
        padding: '6px 12px',
        borderRadius: '8px',
        background: location.pathname === path ? '#e7f5ff' : 'transparent',
        transition: 'all 0.2s',
    })

    return (
        <nav style={{
            background: 'white',
            padding: '0 32px',
            height: '64px',
            display: 'flex',
            alignItems: 'center',
            gap: '8px',
            boxShadow: '0 1px 3px rgba(0,0,0,0.08)',
            position: 'sticky',
            top: 0,
            zIndex: 100,
        }}>
            <span style={{ fontSize: '20px', fontWeight: 700, color: '#212529', marginRight: '16px' }}>
                🍽️ Restaurant
            </span>

            <Link to="/menu" style={linkStyle('/menu')}>Menu</Link>
            {role === 'CLIENT' && <Link to="/orders" style={linkStyle('/orders')}>My Orders</Link>}
            {role === 'KITCHEN' && <Link to="/orders" style={linkStyle('/orders')}>All Orders</Link>}
            {role === 'ADMIN' && <Link to="/admin" style={linkStyle('/admin')}>Admin Panel</Link>}
            {role === 'KITCHEN' && <Link to="/kitchen" style={linkStyle('/kitchen')}>Kitchen</Link>}

            <div style={{ marginLeft: 'auto', display: 'flex', alignItems: 'center', gap: '12px' }}>
                <span style={{
                    background: role === 'ADMIN' ? '#fff3bf' : role === 'KITCHEN' ? '#d3f9d8' : '#e7f5ff',
                    color: role === 'ADMIN' ? '#e67700' : role === 'KITCHEN' ? '#2f9e44' : '#1971c2',
                    padding: '4px 12px',
                    borderRadius: '20px',
                    fontSize: '13px',
                    fontWeight: 600,
                }}>
                    {role}
                </span>
                <button
                    onClick={handleLogout}
                    style={{ background: '#f1f3f5', color: '#495057', padding: '8px 16px' }}
                >
                    Logout
                </button>
            </div>
        </nav>
    )
}

export default Navbar