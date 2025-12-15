import { createContext, useState, useContext } from 'react';
import api from '../api/axios';

const AuthContext = createContext();

export const AuthProvider = ({ children }) => {
    const [user, setUser] = useState(() => {
        const token = localStorage.getItem('token');
        return token ? { username: 'admin', token } : null;
    });

    const [loading, setLoading] = useState(false);

    const login = async (username, password) => {
        try {
            const response = await api.post('/login', { username, password });
            const { token } = response.data;

            localStorage.setItem('token', token);
            setUser({ username, token });
            return true;
        } catch (error) {
            console.error("Login failed", error);
            throw error;
        }
    };

    const logout = () => {
        localStorage.removeItem('token');
        setUser(null);
    };

    return (
        <AuthContext.Provider value={{ user, login, logout, loading }}>
            {children}
        </AuthContext.Provider>
    );
};

// eslint-disable-next-line react-refresh/only-export-components
export const useAuth = () => useContext(AuthContext);