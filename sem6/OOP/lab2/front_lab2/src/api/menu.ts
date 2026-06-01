import apiClient from './client'

export interface MenuItem {
    id: number
    name: string
    description: string
    price: number
    category: string
    available: boolean
}

export const getMenuItems = async (): Promise<MenuItem[]> => {
    const response = await apiClient.get('/menu')
    return response.data
}

export const createMenuItem = async (item: Omit<MenuItem, 'id'>): Promise<MenuItem> => {
    const response = await apiClient.post('/menu', item)
    return response.data
}

export const updateMenuItem = async (id: number, item: Partial<MenuItem>): Promise<MenuItem> => {
    const response = await apiClient.put(`/menu/${id}`, item)
    return response.data
}

export const deleteMenuItem = async (id: number): Promise<void> => {
    await apiClient.delete(`/menu/${id}`)
}